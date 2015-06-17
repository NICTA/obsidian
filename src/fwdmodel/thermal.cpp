// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
//!
//! Contains the implementation for the thermal forward model.
//!
//! \file fwdmodel/thermal.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <glog/logging.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "thermal.hpp"
#pragma GCC diagnostic pop

#include "boost/multi_array.hpp"
#include "world/voxelise.hpp"
#include <iostream>
#include <algorithm>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>

#include <chrono>
typedef std::chrono::high_resolution_clock hrc;

namespace obsidian
{
  namespace fwd
  {
    boost::multi_array<double, 3> fillAndPad(const Eigen::VectorXd& values, const ThermalSpec& spec)
    {
      // The true grid size
      uint nx = spec.voxelisation.xResolution;
      uint ny = spec.voxelisation.yResolution;
      uint nz = spec.voxelisation.zResolution;
      // Make sure we have the right amount of data
      CHECK_EQ(nx * ny * nz, values.size());

      uint bigx = nx + 2 + 2; // top and bottom padding
      uint bigy = ny + 2 + 2; // top and bottom padding
      uint bigz = nz + 1 + 2; // top and bottom padding
      // The multiarrays we'll use
      boost::multi_array<double, 3> volume(boost::extents[bigx][bigy][bigz]);
      // fill in the values to the multiarray
      for (uint i = 0; i < bigx; i++)
      {
        for (uint j = 0; j < bigy; j++)
        {
          for (uint k = 0; k < bigz; k++)
          {
            // first we contrain these to only index the internal region
            // ie no padding. Then we remove the padding offsets so we're
            // actually indexing into the original values matrix
            uint smalli = std::min(std::max(i, (uint) 2), (bigx - 1) - 2) - 2;
            uint smallj = std::min(std::max(j, (uint) 2), (bigy - 1) - 2) - 2;
            uint smallk = std::min(std::max(k, (uint) 1), (bigz - 1) - 2) - 1;
            // Compute the 1D offset into our values matrix
            uint c = smalli * ny * nz + smallj * nz + smallk;
            volume[i][j][k] = values(c);
          }
        }
      }
      return volume;
    }

    boost::multi_array<double, 3> eastWest(const boost::multi_array<double, 3>& pad)
    {
      // The padded grid size
      uint padx = pad.shape()[0];
      uint pady = pad.shape()[1];
      uint padz = pad.shape()[2];
      // new size -- one off all dims and 1 further off the summing dimension
      uint nx = (padx - 1) - 1;
      uint ny = (pady - 1);
      uint nz = (padz - 1);
      boost::multi_array<double, 3> ew(boost::extents[nx][ny][nz]);
      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          for (uint k = 0; k < nz; k++)
          {
            double t1 = pad[i + 1][j][k];
            double t2 = pad[i + 1][j + 1][k];
            double t3 = pad[i + 1][j][k + 1];
            double t4 = pad[i + 1][j + 1][k + 1];
            ew[i][j][k] = (t1 + t2 + t3 + t4) / 4.0;
          }
        }
      }
      return ew;
    }

    boost::multi_array<double, 3> cells(const boost::multi_array<double, 3>& pad, uint axis)
    {
      // The padded grid size
      uint padx = pad.shape()[0];
      uint pady = pad.shape()[1];
      uint padz = pad.shape()[2];
      // bools to switch which axis we're dealing with
      CHECK(axis < 3);
      uint isx = axis == 0;
      uint isy = axis == 1;
      uint isz = axis == 2;
      // new size -- one off all dims and 1 further off the summing dimension
      uint nx = (padx - 1) - isx;
      uint ny = (pady - 1) - isy;
      uint nz = (padz - 1) - isz;
      boost::multi_array<double, 3> ew(boost::extents[nx][ny][nz]);

      // Compute our indexing offsets
      Eigen::MatrixXi indices(4, 3);
      indices << 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1;
      uint xcol = isx * 0 + isy * 1 + isz * 2;
      uint ycol = isx * 2 + isy * 0 + isz * 1;
      uint zcol = isx * 1 + isy * 2 + isz * 0;

      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          for (uint k = 0; k < nz; k++)
          {
            double t1 = pad[i + indices(0, xcol)][j + indices(0, ycol)][k + indices(0, zcol)];
            double t2 = pad[i + indices(1, xcol)][j + indices(1, ycol)][k + indices(1, zcol)];
            double t3 = pad[i + indices(2, xcol)][j + indices(2, ycol)][k + indices(2, zcol)];
            double t4 = pad[i + indices(3, xcol)][j + indices(3, ycol)][k + indices(3, zcol)];
            ew[i][j][k] = (t1 + t2 + t3 + t4) / 4.0;
          }
        }
      }
      return ew;
    }

    boost::multi_array<double, 3> isocells(const boost::multi_array<double, 3>& pad)
    {
      // The padded grid size
      uint padx = pad.shape()[0];
      uint pady = pad.shape()[1];
      uint padz = pad.shape()[2];
      uint nx = padx - 1;
      uint ny = pady - 1;
      uint nz = padz - 1;
      boost::multi_array<double, 3> a(boost::extents[nx][ny][nz]);

      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          for (uint k = 0; k < nz; k++)
          {
            bool px0 = i > 0;
            bool px1 = i < nx - 1;
            bool py0 = j > 0;
            bool py1 = j < ny - 1;
            bool pz0 = k > 0;
            bool pz1 = k < nz - 1;
            bool t1On = (px0 && py0 && pz0);
            bool t2On = (px0 && py0 && pz1);
            bool t3On = (px0 && py1 && pz0);
            bool t4On = (px0 && py1 && pz1);
            bool t5On = (px1 && py0 && pz0);
            bool t6On = (px1 && py0 && pz1);
            bool t7On = (px1 && py1 && pz0);
            bool t8On = (px1 && py1 && pz1);
            double t1 = t1On * pad[i][j][k];
            double t2 = t2On * pad[i][j][k + 1];
            double t3 = t3On * pad[i][j + 1][k];
            double t4 = t4On * pad[i][j + 1][k + 1];
            double t5 = t5On * pad[i + 1][j][k];
            double t6 = t6On * pad[i + 1][j][k + 1];
            double t7 = t7On * pad[i + 1][j + 1][k];
            double t8 = t8On * pad[i + 1][j + 1][k + 1];
            double norm = (t1On + t2On + t3On + t4On + t5On + t6On + t7On + t8On);
            a[i][j][k] = (t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8) / (8.0 * norm);
          }
        }
      }
      return a;
    }

    boost::multi_array<double, 3> temp(const boost::multi_array<double, 3>& eastwest, const boost::multi_array<double, 3>& northsouth,
                                       const boost::multi_array<double, 3>& updown, const boost::multi_array<double, 3>& sCells,
                                       const Eigen::MatrixXd& tempZ0, const Eigen::MatrixXd& zLowBound, bool zLowBoundIsHeatFlow,
                                       double xSize, double ySize, double zSize)
    {
      //remove the padding from sCells
      uint nx = sCells.shape()[0] - 2;
      uint ny = sCells.shape()[1] - 2;
      uint nz = sCells.shape()[2] - 2;
      uint n = nx * ny * nz; // total n

      double dx = xSize / double(nx - 1); // original number of cells
      double dy = ySize / double(ny - 1); // original number of cells
      double dz = zSize / double(nz);

      Eigen::SparseMatrix<double> A(n, n);  // big sparse matrix
      std::vector<Eigen::Triplet<double>> coeffs;
      Eigen::VectorXd b = Eigen::VectorXd::Zero(n); // constant term

      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          for (uint k = 0; k < nz; k++)
          {
            uint row = i * ny * nz + j * nz + k;
            uint col = row;
            b(row) = -1 * sCells[i + 1][j + 1][k + 1]; //ignores the outer layer of sCells

            double val_west = eastwest[i][j + 1][k + 1];
            double val_east = eastwest[i + 1][j + 1][k + 1];
            double val_north = northsouth[i + 1][j][k + 1];
            double val_south = northsouth[i + 1][j + 1][k + 1];
            double val_up = updown[i + 1][j + 1][k];
            double val_down = updown[i + 1][j + 1][k + 1];

            double z_up_contrib = val_up / (dz * dz);
            double z_down_contrib = val_down / (dz * dz);
            double y_north_contrib = val_north / (dy * dy);
            double y_south_contrib = val_south / (dy * dy);
            double x_west_contrib = val_west / (dx * dx);
            double x_east_contrib = val_east / (dx * dx);

            double sum = -1 * (z_up_contrib + z_down_contrib + y_north_contrib + y_south_contrib + x_west_contrib + x_east_contrib);
            // Assign to a
            coeffs.push_back( { row, col, sum });
            // Z-axis contribution
            // Top
            if (k == 0)
            {
              b(row) = b(row) - z_up_contrib * tempZ0(i, j);
            } else
            {
              col = i * ny * nz + j * nz + k - 1;
              coeffs.push_back( { row, col, z_up_contrib });
            }

            // Bottom
            if (k == nz - 1)
            {
              if (zLowBoundIsHeatFlow)
              {
                col = row;
                coeffs.push_back( { row, col, z_down_contrib });
                b(row) = b(row) - zLowBound(i, j) / dz;
              } else
              {
                b(row) = b(row) - z_down_contrib * zLowBound(i, j);
              }
            } else
            {
              col = i * ny * nz + j * nz + k + 1;
              coeffs.push_back( { row, col, z_down_contrib });
            }

            // Y-axis contribution
            if (j == 0)
            {
              col = i * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, y_north_contrib });
            } else
            {
              col = i * ny * nz + (j - 1) * nz + k;
              coeffs.push_back( { row, col, y_north_contrib });
            }
            if (j == ny - 1)
            {
              col = i * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, y_south_contrib });
            } else
            {
              col = i * ny * nz + (j + 1) * nz + k;
              coeffs.push_back( { row, col, y_south_contrib });
            }
            // X-axis contribution
            if (i == 0)
            {
              col = i * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, x_west_contrib });
            } else
            {
              col = (i - 1) * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, x_west_contrib });
            }
            if (i == nx - 1)
            {
              col = i * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, x_east_contrib });
            } else
            {
              col = (i + 1) * ny * nz + j * nz + k;
              coeffs.push_back( { row, col, x_east_contrib });
            }

          }
        }
      }

      A.setFromTriplets(coeffs.begin(), coeffs.end());

      // solve for -A
      A = -1 * A;
      b = -1 * b;

      Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;
      solver.setTolerance(1.0e-4); // tested using demo3dworld - should be +- 0.5 degrees under typical use @ 40*24*32
      solver.compute(A);

      if (solver.info() != Eigen::Success)
      {
        LOG(ERROR)<< "Matrix decomposition failed";
      }
      Eigen::VectorXd tvec(n);
      tvec = solver.solve(b);
      if (solver.info() != Eigen::Success)
      {
        LOG(ERROR)<< "Linear system could not be solved";
      }
      boost::multi_array<double, 3> t(boost::extents[nx][ny][nz + 1]);
      // add the surface temperature

      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          t[i][j][0] = tempZ0(i, j);
        }
      }
      // count from z=1
      uint c = 0;
      for (uint i = 0; i < nx; i++)
      {
        for (uint j = 0; j < ny; j++)
        {
          for (uint k = 1; k < nz + 1; k++)
          {
            t[i][j][k] = tvec(c);
            c++;
          }
        }
      }
      return t;
    }

    uint clip(int in, uint maxX)
    {
      uint x = (in > 0) * in;
      x = maxX - (x < maxX) * (maxX - x);
      return (uint) x;
    }

    Eigen::VectorXd evalAtLocations(const boost::multi_array<double, 3>& tempVox, const Eigen::MatrixXd& locations,
                                    const std::pair<double, double>& xSize, const std::pair<double, double>& ySize,
                                    const std::pair<double, double>& zSize)
    {
      uint nx = tempVox.shape()[0];
      uint ny = tempVox.shape()[1];
      uint nz = tempVox.shape()[2];
      double x0 = xSize.first;
      double y0 = ySize.first;
      double z0 = zSize.first;
      double deltaX = xSize.second - x0;
      double deltaY = ySize.second - y0;
      double deltaZ = zSize.second - z0;
      Eigen::VectorXd temps(locations.rows());
      for (uint i = 0; i < locations.rows(); i++) //AAA
      {
        // Clip rather than check
        double xx = (double) (nx - 1) * (locations(i, 0) - x0) / deltaX;
        double yy = (double) (ny - 1) * (locations(i, 1) - y0) / deltaY;
        double zz = (double) (nz - 1) * (locations(i, 2) - z0) / deltaZ;

        // Clip so we get continuous edge conditions after the image
        uint x1 = clip(xx, nx - 1);
        uint x2 = clip(xx + 1, nx - 1);
        uint y1 = clip(yy, ny - 1);
        uint y2 = clip(yy + 1, ny - 1);
        uint z1 = clip(zz, nz - 1);
        uint z2 = clip(zz + 1, nz - 1);

        double alpha2 = xx - (double) x1;
        double beta2 = yy - (double) y1;
        double gamma2 = zz - (double) z1;
        double alpha1 = 1.0 - alpha2;
        double beta1 = 1.0 - beta2;
        double gamma1 = 1.0 - gamma2;

        /*Eigen::VectorXi pos(6);
         pos << xx,yy,zz, (alpha1*(double)x1 + alpha2*(double)x2),(beta1*(double)y1 + beta2*(double)y2),(gamma1*(double)z1 + gamma2*(double)z2);
         std::cout << pos.transpose() <<"\n";
         */

        // who needs loops?
        temps(i) = alpha1 * beta1 * gamma1 * tempVox[x1][y1][z1] + alpha1 * beta1 * gamma2 * tempVox[x1][y1][z2]
            + alpha1 * beta2 * gamma1 * tempVox[x1][y2][z1] + alpha1 * beta2 * gamma2 * tempVox[x1][y2][z2]
            + alpha2 * beta1 * gamma1 * tempVox[x2][y1][z1] + alpha2 * beta1 * gamma2 * tempVox[x2][y1][z2]
            + alpha2 * beta2 * gamma1 * tempVox[x2][y2][z1] + alpha2 * beta2 * gamma2 * tempVox[x2][y2][z2]; //ouch
      }
      return temps;
    }

    template<>
    ThermalCache generateCache<ForwardModel::THERMAL>(const std::vector<world::InterpolatorSpec>& boundaryInterpolation,
                                                      const WorldSpec& worldSpec, const ThermalSpec& thermSpec)
    {
      LOG(INFO)<< "Caching thermal sensitivity...";
      const VoxelSpec& thermVox = thermSpec.voxelisation;
      world::Query thermQuery(boundaryInterpolation, worldSpec, thermVox.xResolution, thermVox.yResolution, thermVox.zResolution,
                              world::SamplingStrategy::noAA);
      return { boundaryInterpolation, thermQuery,
               worldSpec.xBounds, worldSpec.yBounds, worldSpec.zBounds};
    }

    template<>
    ThermalResults forwardModel<ForwardModel::THERMAL>(const ThermalSpec& spec, const ThermalCache& cache, const WorldParams& world)
    {
      Eigen::MatrixXd cond = world::getVoxels(cache.boundaryInterpolation, world, cache.query, RockProperty::ThermalConductivity); // conductivity shouldnt go to zero...
      Eigen::MatrixXd prod = world::getVoxels(cache.boundaryInterpolation, world, cache.query, RockProperty::ThermalProductivity);

      auto volume = fillAndPad(world::flatten(cond), spec);
      auto eastwest = cells(volume, 0);
      auto northsouth = cells(volume, 1);
      auto updown = cells(volume, 2);
      auto sVolume = fillAndPad(world::flatten(prod), spec);
      auto sCells = isocells(sVolume);

      uint nx = spec.voxelisation.xResolution;
      uint ny = spec.voxelisation.yResolution;
      Eigen::MatrixXd tempZ0 = Eigen::MatrixXd::Ones(nx + 1, ny + 1) * spec.surfaceTemperature;
      Eigen::MatrixXd zLowBound = Eigen::MatrixXd::Ones(nx + 1, ny + 1) * spec.lowerBoundary;
      bool isHeatFlow = spec.lowerBoundaryIsHeatFlow;
      double xSize = cache.xBounds.second - cache.xBounds.first;
      double ySize = cache.yBounds.second - cache.yBounds.first;
      double zSize = cache.zBounds.second - cache.zBounds.first;
      boost::multi_array<double, 3> tempVox = temp(eastwest, northsouth, updown, sCells, tempZ0, zLowBound, isHeatFlow, xSize, ySize,
                                                   zSize);

      ThermalResults results;
      results.readings = evalAtLocations(tempVox, spec.locations, cache.xBounds, cache.yBounds, cache.zBounds);
      return results;
    }

  }
}
