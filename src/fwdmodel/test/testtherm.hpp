// Copyright (c) 2014, NICTA. 
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

#include <gtest/gtest.h>

#include "fwdmodel/thermal.hpp"

namespace obsidian
{
  namespace fwd
  {
    class ThermTest: public testing::Test
    {
    public:

      ThermalSpec spec;
      Eigen::VectorXd values;
      boost::multi_array<double, 3> trueVolume;
      uint nx = 3;
      uint ny = 3;
      uint nz = 3;
      Eigen::MatrixXd sliceX0;
      Eigen::MatrixXd sliceX1;
      Eigen::MatrixXd sliceX2;
      Eigen::MatrixXd sliceX3;
      Eigen::MatrixXd sliceX4;

      ThermTest()
          : trueVolume(boost::extents[nx + 2][ny + 2][nz + 1]),
            sliceX0(ny + 2, nz + 1),
            sliceX1(ny + 2, nz + 1),
            sliceX2(ny + 2, nz + 1),
            sliceX3(ny + 2, nz + 1),
            sliceX4(ny + 2, nz + 1)
      {
        spec.voxelisation.xResolution = nx;
        spec.voxelisation.yResolution = ny;
        spec.voxelisation.zResolution = nz;
        // values = Eigen::VectorXd::Ones(nx * ny * nz);
        values = Eigen::VectorXd::LinSpaced(nx * ny * nz, 1, nx * ny * nz);
      }

    };

    bool sliceAgainstMatrix(const Eigen::MatrixXd m, const boost::multi_array<double, 3> vol, uint xIdx)
    {
      bool match = true;
      for (uint i = 0; i < m.rows(); i++)
      {
        for (uint j = 0; j < m.cols(); j++)
        {
          match = m(i, j) == vol[xIdx][i][j];
          if (!match)
            break;
        }
      }
      return match;
    }

    void print(const boost::multi_array<double, 3>& a)
    {
      for (uint i = 0; i < a.shape()[0]; i++)
      {
        std::cout << "x = " << i << std::endl;
        for (uint j = 0; j < a.shape()[1]; j++)
        {
          for (uint k = 0; k < a.shape()[2]; k++)
          {
            std::cout << a[i][j][k] << ",";
          }
          std::cout << std::endl;
        }
        std::cout << std::endl;
      }
    }

  TEST_F(ThermTest, fill)
  {
    WorldSpec wspec;
    wspec.xBounds = std::make_pair(0.0, 1.0);
    wspec.yBounds = std::make_pair(0.0, 1.0);
    wspec.zBounds = std::make_pair(0.0, 1.0);

    // auto volume = fillAndPad(values, spec);
    // auto eastwest = cells(volume, 0);
    // auto northsouth = cells(volume, 1);
    // auto updown = cells(volume, 2);
    // auto sCells =  isocells(volume);
    // Eigen::MatrixXd tempZ0 = Eigen::MatrixXd::Ones(4,4)*273.15;
    // Eigen::MatrixXd zLowBound = Eigen::MatrixXd::Ones(4,4)*500.0;
    // bool isHeatFlow = false;
    // auto t = temp(eastwest, northsouth, updown, sCells, tempZ0, zLowBound, isHeatFlow,wspec);
    // print(t);
  }
}
}
