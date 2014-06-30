//!
//! Contains common functions used by both gravity and magnetic forward models.
//!
//! \file fwdmodel/gravmag.cpp
//! \author Darren Shen
//! \author Alistair Reid
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "fwdmodel/gravmag.hpp"
#include <cmath>
#include <boost/multi_array.hpp>
#include "world/voxelise.hpp"
#include <glog/logging.h>

namespace obsidian
{
  namespace fwd
  {
    GravmagInterpolatorParams makeInterpParams(const VoxelSpec& voxelisation, const Eigen::MatrixXd& locations, const WorldSpec& worldSpec)
    {
      // The interpolation weights
      uint nQuery = locations.rows();
      uint imWidth = voxelisation.xResolution + 2;  // outer grid!
      uint imHeight = voxelisation.yResolution + 2; // outer grid!
      double imMaxX = imWidth - 1; // image coordinates are 0 to xResolution-1
      double imMaxY = imHeight - 1;

      double sensorZ = locations(0, 2);
      Eigen::MatrixXi sensorIndices(nQuery, 4);
      Eigen::MatrixXd sensorWeights(nQuery, 4);
      double halfGridX = (worldSpec.xBounds.second - worldSpec.xBounds.first) / (2.0 * voxelisation.xResolution);
      double halfGridY = (worldSpec.yBounds.second - worldSpec.yBounds.first) / (2.0 * voxelisation.yResolution);
      double L = worldSpec.xBounds.first - halfGridX; // switching to outer grid!
      double R = worldSpec.xBounds.second + halfGridX;
      double T = worldSpec.yBounds.first - halfGridY;
      double B = worldSpec.yBounds.second + halfGridY;

      uint nGrid = imWidth * imHeight; // number of max possible inducing query points
      std::vector<uint> gridIndx(nGrid);  // store a map from grid index to inuse key
      for (uint j = 0; j < nGrid; j++)
        gridIndx[j] = 0; // initialise to zero (unused)

      std::vector<std::pair<uint, uint>> cell2grid; // the [INUSEth] element is the <i,j> coordinate on the grid

      for (uint i = 0; i < nQuery; i++)
      {
        // Note: this needs a refactor as it is used in more than one place
        // Get [double] position in the image - ie if there are
        double xx = imMaxX * (locations(i, 0) - L) / (R - L); // only reaches imMaxX at very final edge
        double yy = imMaxY * (locations(i, 1) - T) / (B - T);
        CHECK(locations(i, 2) == sensorZ); // if this is a problem, then you
        // must have different sensor heights -> can use the commented out stub in gravity to fix

        // Clip so we get continuous edge conditions after the image
        xx = (xx > 0) * xx;
        xx = imMaxX - (xx < imMaxX) * (imMaxX - xx);
        yy = (yy > 0) * yy;
        yy = imMaxY - (yy < imMaxY) * (imMaxY - yy);

        // Get the components for the linear interp
        uint x1 = std::floor(xx);
        uint y1 = std::floor(yy);
        uint x2 = x1 + (xx < imMaxX);
        uint y2 = y1 + (yy < imMaxY);
        double alpha = xx - (double) x1;
        double beta = yy - (double) y1;

        // The order here is x1y1 x2y1 x1y2 x2y2
        sensorWeights(i, 0) = (1 - alpha) * (1 - beta);
        sensorWeights(i, 1) = alpha * (1 - beta);
        sensorWeights(i, 2) = (1 - alpha) * beta;
        sensorWeights(i, 3) = alpha * beta;

        // Dynamically index-map
        // The order here is the same: x1y1 x2y1 x1y2 x2y2
        uint xind[4] = { x1, x2, x1, x2 };
        uint yind[4] = { y1, y1, y2, y2 };
        for (int k = 0; k < 4; k++)
        {
          uint gIndex = xind[k] + yind[k] * imWidth;
          if (gridIndx[gIndex] == 0) // not initialised
          {
            cell2grid.push_back(std::make_pair(xind[k], yind[k]));
            gridIndx[gIndex] = cell2grid.size();
          }
          sensorIndices(i, k) = gridIndx[gIndex] - 1; // zero is starting condition
        }
      }

      // Reconstitute the gridLocations
      uint nGridQry = cell2grid.size();
      Eigen::MatrixXd gridLocations(nGridQry, 3);
      for (uint k = 0; k < nGridQry; k++)
      {
        gridLocations(k, 0) = ((double) cell2grid[k].first) * (R - L) / ((double) imMaxX) + L;
        gridLocations(k, 1) = ((double) cell2grid[k].second) * (B - T) / ((double) imMaxY) + T;
        gridLocations(k, 2) = sensorZ;
      }

      return
      { sensorIndices, sensorWeights, gridLocations}; // interpParams
    }

    Eigen::VectorXd computeField(const Eigen::MatrixXd &sens, const Eigen::MatrixXi sensorIndices, const Eigen::MatrixXd sensorWeights,
                                 const Eigen::VectorXd &properties)
    {
      Eigen::VectorXd rawField = sens * properties;
      uint nQuery = sensorWeights.rows();
      Eigen::VectorXd outField(nQuery);
      for (uint i = 0; i < nQuery; i++)
      {
        // The 4 components of the linear interp
        outField(i) = 0.0;
        for (uint j = 0; j < 4; j++)
          outField(i) += rawField(sensorIndices(i, j)) * sensorWeights(i, j);
      }
      return outField;
    }

    namespace detail
    {
      //! Data structure to represent a 3D array.
      //!
      typedef boost::multi_array<double, 3> Array3d;

      //! Compute the sensitivity integral at a particular point in the mesh
      //! 
      //! \param i, j, k The indices of the mesh point
      //! \param eZ The 3D array containing the sensitivities at each mesh grid point
      //!
      double computeSensIntegral(int i, int j, int k, const Array3d &eZ)
      {
        return (eZ[i + 1][j + 1][k + 1] - eZ[i + 1][j + 1][k] - eZ[i + 1][j][k + 1] + eZ[i + 1][j][k])
            - (eZ[i][j + 1][k + 1] - eZ[i][j + 1][k] - eZ[i][j][k + 1] + eZ[i][j][k]);
      }

      Eigen::MatrixXd computeSensitivity(const Eigen::VectorXd &xEdges, const Eigen::VectorXd &yEdges, const Eigen::VectorXd &zEdges,
                                         const Eigen::VectorXd &xField, const Eigen::VectorXd &yField, const Eigen::VectorXd &zField,
                                         const Eigen::MatrixXd &locations,
                                         double (*sensFunc)(double, double, double, double, double, double))
      {
        // Hold the calculation values at each point (or corner of prism)
        detail::Array3d eZ(boost::extents[xEdges.rows()][yEdges.rows()][zEdges.rows()]);

        // The sensitivity matrix contains the sensitivity for each location and prism
        int nPrisms = (xEdges.rows() - 1) * (yEdges.rows() - 1) * (zEdges.rows() - 1);
        Eigen::MatrixXd sens(locations.rows(), nPrisms);

        // For each sensor location
        for (int n = 0; n < locations.rows(); n++)
        {
          // Shift the coordinates so that the sensor location is at the origin
          Eigen::VectorXd x = xEdges.array() - locations(n, 0);
          Eigen::VectorXd y = yEdges.array() - locations(n, 1);
          Eigen::VectorXd z = -(zEdges.array() - locations(n, 2)); // flip z-axis

          // Lazy edge padding for both grav and mag
          double aLongWay = 1e5; // Metres, chosen compromise
          x(0) -= aLongWay;
          y(0) -= aLongWay;
          x(x.rows() - 1) += aLongWay;
          y(y.rows() - 1) += aLongWay;

          // Precompute eZ for each point
          for (int i = 0; i < x.rows(); ++i)
            for (int j = 0; j < y.rows(); ++j)
              for (int k = 0; k < z.rows(); ++k)
                // Cache the value for this point
                eZ[i][j][k] = sensFunc(x(i), y(j), z(k), xField(i), yField(j), zField(k));

          // Compute the sensitivities
          int idx = 0;
          for (int i = 0; i < xEdges.rows() - 1; ++i)
            for (int j = 0; j < yEdges.rows() - 1; ++j)
              for (int k = 0; k < zEdges.rows() - 1; ++k)
                sens(n, idx++) = -detail::computeSensIntegral(i, j, k, eZ);
        }

        return sens;
      }
    } // namespace detail
  } // namespace fwd
} // namespace obsidian
