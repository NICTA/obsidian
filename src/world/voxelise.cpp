//!
//! Contains the implementation for grid voxelisation.
//!
//! \file world/voxelise.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "voxelise.hpp"
#include "transitions.hpp"
#include "datatype/datatypes.hpp"
#include "world/property.hpp"

namespace obsidian
{
  namespace world
  {
    Eigen::MatrixXd getVoxels(const std::vector<world::InterpolatorSpec>& interpolators,
        const WorldParams& inputs, const Query& query, obsidian::RockProperty desiredProp)
    {
      // Query q(query);
      // First: get the transitions
      Eigen::MatrixXd transitions = getTransitions(interpolators, inputs, query);

      // Then propertise them!
      Eigen::VectorXd props = extractProperty(inputs, desiredProp);
      Eigen::MatrixXd properties = voxelise(transitions,query.edgeZ, props);
      return properties;
    }

    Eigen::MatrixXd voxelise(const Eigen::MatrixXd &transitions,
        const Eigen::VectorXd &zIntercepts, const Eigen::VectorXd &props)
    {
      // Note: layerVals(z, i) is the property value of voxel i

      // Define these for code readability below
      uint nCellsVert = zIntercepts.rows()-1; // 1 less cell than number of edges...
      uint nTransitions = transitions.rows();
      uint nQuery = transitions.cols();
      uint finalLayer = nTransitions-1;
      Eigen::MatrixXd layerVals(nCellsVert, nQuery); // Output matrix

      // Do each query sequentially
      for (uint i = 0; i < nQuery; i++)
      {
        uint thisLayer = 0; // between transitions [thisLayer, thisLayer+1]
        double nextTransition = transitions(thisLayer+1, i);

        // Over each cell
        for (uint z = 0; z < nCellsVert; ++z)
        {
          double lastZVal = zIntercepts(z);   // top of cell
          double thisZVal = zIntercepts(z+1); // bottom of cell

          if ((thisLayer == finalLayer) || (thisZVal < nextTransition))
          {
            // cell contains no transitions
            layerVals(z, i) = props(thisLayer);
          }
          else
          {
            double totalVal = 0.0; // Accumulate an integral *unnormalised*
            double lastTransition = lastZVal; // Between piecewise thresholds
            while (nextTransition <= thisZVal)
            {
              totalVal += props(thisLayer)*(nextTransition-lastTransition);
              thisLayer++;
              lastTransition = nextTransition; // which is <= zIntercepts(z)
              if (thisLayer == finalLayer)
                break; // and we can never be in this else block again...
              nextTransition = transitions(thisLayer+1, i);
            }
            // Take into account the unused portion of the current layer
            totalVal += props(thisLayer)*(thisZVal - lastTransition);

            // And normalise the weightings
            layerVals(z, i) = totalVal / (thisZVal - lastZVal);
          }
        }
      }

      return layerVals;
    }

    Eigen::VectorXd shrink3d(const Eigen::VectorXd &densities, int nx, int ny, int nz)
    {
      typedef boost::multi_array<double, 3> Array3d;
      Array3d d(boost::extents[nx][ny][nz]);

      // Remap the vector into a 3d array
      int n = 0;
      for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
          for (int k = 0; k < nz; k++)
            d[i][j][k] = densities(n++);

      Eigen::VectorXd result((nx / 2) * (ny / 2) * (nz / 2));

      // Remap to a smaller vector, taking the mean densities of the neighbouring points
      n = 0;
      for (int i = 0; i < nx; i += 2)
        for (int j = 0; j < ny; j += 2)
          for (int k = 0; k < nz; k += 2)
            result(n++) =
              (d[i][j][k] + d[i+1][j][k] + d[i][j+1][k] + d[i+1][j+1][k] +
               d[i][j][k+1] + d[i+1][j][k+1] + d[i][j+1][k+1] + d[i+1][j+1][k+1]) / 8;

      return result;
    }
  }
}
