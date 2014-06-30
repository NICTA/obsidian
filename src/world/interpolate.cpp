//!
//! Contains the implementation for interpolation.
//!
//! \file world/interpolate.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "world/interpolate.hpp"
#include "world/grid.hpp"
#include "world/kernel.hpp"

#include <glog/logging.h>

namespace obsidian
{
  namespace world
  {
    std::vector<InterpolatorSpec> worldspec2Interp(const WorldSpec& region)
    {
      uint nBoundaries = region.boundaries.size();
      std::vector<InterpolatorSpec> interpolators(nBoundaries);
      for (uint i=0; i<nBoundaries; i++)
      {
        interpolators[i] = InterpolatorSpec(region.xBounds, region.yBounds, region.zBounds, region.boundaries[i]);
      }
      return interpolators;
    }

    Eigen::VectorXd kernelInterpolate(const Query& query, const uint boundary, const Eigen::MatrixXd& input)
    {
      Eigen::VectorXd flatInput = world::flatten(input);
      return query.interpolatorWeights[boundary]*flatInput;
    }

    Eigen::VectorXd linearInterpolate(const Query& query, InterpolatorSpec interpolator)
    {
      uint nQuery = query.positionXY.rows();
      CHECK(query.positionXY.cols() == 2);

      double imWidth  = interpolator.offsetFunction.rows();
      double imHeight = interpolator.offsetFunction.cols();
      double imMaxX = imWidth-1;
      double imMaxY = imHeight-1;
      double L = interpolator.offsetXBounds.first;
      double R = interpolator.offsetXBounds.second;
      double T = interpolator.offsetYBounds.first;
      double B = interpolator.offsetYBounds.second;
      double scaleW = (imWidth)/(R-L);
      double scaleH = (imHeight)/(B-T);

      Eigen::VectorXd output(nQuery);

      for (uint i=0; i<nQuery; i++)
      {
        double xx = (query.positionXY(i,0)-L)*scaleW;
        double yy = (query.positionXY(i,1)-T)*scaleH;
        // Clip so we get continuous edge conditions after the image
        xx = (xx>0)*xx;
        xx = imMaxX - (xx<imMaxX)*(imMaxX-xx);
        yy = (yy>0)*yy;
        yy = imMaxY - (yy<imMaxY)*(imMaxY-yy);

        // Get the components for the linear interp
        uint x1 = std::floor(xx);
        uint y1 = std::floor(yy);
        uint x2 = x1 + (xx<imMaxX);
        uint y2 = y1 + (yy<imMaxY);
        double alphaX = xx - (double)x1;
        double alphaY = yy - (double)y1;


        double v11 = interpolator.offsetFunction(x1,y1);
        double v21 = interpolator.offsetFunction(x2,y1);
        double v12 = interpolator.offsetFunction(x1,y2);
        double v22 = interpolator.offsetFunction(x2,y2);

        // compute the linear interpolation
        output(i) = (1.0-alphaX)*( (1.0-alphaY)*v11 + alphaY*v12 ) + alphaX*( (1.0-alphaY)*v21 + alphaY*v22);
      }
      return output;
    }

  }
}
