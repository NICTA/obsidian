//!
//! Contains objects and methods for querying.
//!
//! \file world/query.hpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Core>
#include "datatype/world.hpp"
#include "world/interpolatorspec.hpp"
#include "world/grid.hpp"

namespace obsidian
{
  namespace world
  {
    //! Represents a sampling strategy for queries.
    //!
    enum SamplingStrategy 
    {
      //! Sample with no anti-aliasing.
      noAA, 

      //! Supersample at twice the resolution.
      superSamp2X, 

      //! Supersample at four times the resolution.
      superSamp4X
    };

    // We could call getTransitions or getVoxels on this...
    struct Query
    {
      //! Create a new query.
      //!
      //! \param boundaries List of interpolator specifications.
      //! \param region The world specifications.
      //! \param resX The resolution in the X direction.
      //! \param resY The resolution in the X direction.
      //! \param resZ The resolution in the X direction.
      //! \param sampling The sampling strategy used.
      //!
      Query(const std::vector<InterpolatorSpec>& boundaries, WorldSpec region,
          uint resX, uint resY, uint resZ, SamplingStrategy sampling = SamplingStrategy::noAA)
        :resX(resX), resY(resY), resZ(resZ), sampling(sampling)
      {
        // auto-build a query for the region...
        // Patch into existing code
        Eigen::Vector2d startPt(region.xBounds.first, region.yBounds.first);
        Eigen::Vector2d endPt(region.xBounds.second, region.yBounds.second);
        Eigen::Vector2i resolution(resX, resY);

        positionXY = internalGrid2DX(region.xBounds, region.yBounds, resX, resY);

        edgeX  = Eigen::VectorXd::LinSpaced(resX+1, region.xBounds.first, region.xBounds.second);
        edgeY  = Eigen::VectorXd::LinSpaced(resY+1, region.yBounds.first, region.yBounds.second);
        edgeZ  = Eigen::VectorXd::LinSpaced(resZ+1, region.zBounds.first, region.zBounds.second);

        cacheInitialised = false;
        boundariesAreTimes = region.boundariesAreTimes;
        initInterpolatorWeights(boundaries);
      }

      // MT needs a scatter sample
      // note:locations should be n*2
      Query(const std::vector<InterpolatorSpec>& boundaries, WorldSpec region, Eigen::MatrixXd locations)
       : positionXY(locations), cacheInitialised(false)
      {
        boundariesAreTimes = region.boundariesAreTimes;
        initInterpolatorWeights(boundaries);
      }

      //! Initialise interpolator weights.
      //!
      //! \param boundaries List of interpolator specifications.
      //!
      void initInterpolatorWeights(const std::vector<InterpolatorSpec>& boundaries)
      {
        for (const InterpolatorSpec & boundary : boundaries)
        {
          interpolatorWeights.push_back(boundary.getWeights(positionXY).transpose());
        }
      }

      //! Default constructor for global objects
      Query(){}

      //! Get the number of points in the query.
      //!
      //! \return The number of points in the query.
      //!
      int numPoints() const
      {
        return positionXY.rows();
      }

      //! The control point resolution in the x direction (longitude).
      uint resX;

      //! The control point resolution in the y direction (latitude).
      uint resY;

      //! The control point resolution in the z direction (depth).
      uint resZ;

      //! 
      Eigen::MatrixXd positionXY;

      //! A vector containing the 
      Eigen::VectorXd edgeX;

      //!
      Eigen::VectorXd edgeY;

      //!
      Eigen::VectorXd edgeZ;

      //!
      SamplingStrategy sampling;

      //! Flag indicating whether the cache has been initialised
      bool cacheInitialised;

      //! 
      bool boundariesAreTimes;

      //!
      std::vector<Eigen::MatrixXd> interpolatorWeights;
    };
  }
}
