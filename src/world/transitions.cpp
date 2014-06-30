//!
//! Contains the implementation of querying layer transitions.
//!
//! \file world/transitions.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "world/transitions.hpp"

namespace obsidian
{
  namespace world
  {
    Eigen::MatrixXd getTransitions(const std::vector<world::InterpolatorSpec>& region, const WorldParams& inputs, const Query& query)
    {
      uint nBoundaries = region.size();
      uint nQuery = query.numPoints();
      Eigen::MatrixXd transitions(nBoundaries, nQuery);
      Eigen::VectorXd lastTransition; // the previous transition
      Eigen::VectorXd transitioni = Eigen::VectorXd::Zero(nQuery); // special transition...
      double floorHeight = region[0].floorHeight; // assume same for all
      const std::vector<Eigen::MatrixXd> &ctrlPts = inputs.controlPoints;
      //bool build_cache = !query.cacheInitialised;
      Eigen::VectorXd last_offset;
      for (uint i = 0; i < nBoundaries; i++)
      {
        lastTransition = transitioni;
        // We pass i into kernelInterpolate so it knows which weights to cache
        transitioni = kernelInterpolate(query, i, ctrlPts[i]);
        // clip

        // Add in the mean function here - need to change interpolator spec
        // its input free (we could really cache this per layer per query!)
        Eigen::VectorXd offseti = linearInterpolate(query, region[i]);

        if (query.boundariesAreTimes)
        {
          if (i > 0)
          {
            offseti = last_offset
              + offseti * inputs.rockProperties[i - 1][static_cast<uint>(RockProperty::PWaveVelocity)];
          }

          last_offset = offseti;
        }

        // Add the mean
        transitioni = (transitioni.array() + offseti.array()).matrix();

        // Clip
        transitioni = transitioni.cwiseMax(lastTransition);
        transitioni = transitioni.array().min(floorHeight);

        if (region[i].boundaryClass == obsidian::BoundaryClass::Warped)
          transitioni = postProcessGranites(transitioni, offseti, lastTransition, query,  i, inputs.controlPoints[i], floorHeight);

        // Enforce clipping between rows
        transitions.row(i) = transitioni;

      }
      // Now we can mark it as cached :)
      //query.cacheInitialised = true;

      return transitions;
    }

    Eigen::MatrixXd thickness(const Eigen::MatrixXd& transitions)
    {
      uint mqueries = transitions.cols();
      uint nLayers = transitions.rows();

      // Note this matrix is transposed relative to the input matrix
      Eigen::MatrixXd  thicknesses(mqueries,nLayers);

      for (uint sensorID=0;sensorID<mqueries;sensorID++)
      {
        for (uint layerID = 0; layerID<nLayers;layerID++)
        {
          if (layerID != nLayers-1)
          {
            thicknesses(sensorID,layerID) = transitions(layerID+1,sensorID)-transitions(layerID,sensorID);
          }
          else
          {
            //The last layer is a halfbox so its thickness is infinite
            thicknesses(sensorID,layerID) = std::numeric_limits<double>::infinity(); 
          }
        }
      }
      return thicknesses;
    }

    Eigen::MatrixXd postProcessGranites(const Eigen::MatrixXd& transitionI, Eigen::VectorXd offseti, const Eigen::MatrixXd& transitionU,
        const Query& query, const uint interface, const Eigen::MatrixXd& input, double floorHeight)
    {
      // Step 1: hack the inputs REQUIRING gridded control points
      Eigen::MatrixXd inputB = input;
      int height = input.rows();
      int width = input.cols();

      // Artificially lower the local control point pattern...
      // Vertical

      for (int i=0; i< height; i++)
      {
        for (int j=0; j< width; j++)
        {
          int L = j-1, R = j+1, U = i-1, D = i+1;
          L = (L>0)?L:0;
          R = (R<width)?R:width-1;
          U = (U>0)?U:0;
          D = (D<height)?D:height-1;
          double value = 0;
          for (int ii = U; ii<=D; ii++)
          {
            for (int jj = L; jj<=R; jj++)
            {
              double thisVal = input(ii,jj);
              if (thisVal>value)
                value=thisVal;
            }
          }

          inputB(i,j) = value;
        }
      }

      Eigen::MatrixXd transitionL = world::kernelInterpolate(query, interface, inputB);
      transitionL = (transitionL.array() + offseti.array()).matrix();
      transitionL = transitionL.array().min(floorHeight);

      Eigen::ArrayXd transitionLarray = transitionL.array().max(transitionU.array());
      Eigen::ArrayXd transitionIarray = (transitionI.array().max(transitionU.array()));
      Eigen::ArrayXd hField = transitionLarray - transitionU.array();
      Eigen::ArrayXd gField = transitionIarray - transitionU.array();
      gField = gField.min(hField);
      hField = hField + 1e-3; // Otherwise it is possible to get NANs as hField approaches zero
      return (transitionU.array() + hField*((1.0-(1.0-(gField/hField)).square()).max(0).sqrt())).matrix();
    }
  }
}
