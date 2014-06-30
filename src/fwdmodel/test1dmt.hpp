// Copyright (c) 2013, NICTA. 
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

#include <gtest/gtest.h>
#include <cmath>
#include <Eigen/Dense>

#include "fwdmodel/mt1d.hpp"

using namespace obsidian;
using namespace obsidian::fwd;

namespace test
{
  namespace fwd
  {
    class MtAnisoScenario: public ::testing::Test
    {
    protected:
      MtAnisoScenario()
          : interfaces(0, 88, 2200),
            thicknesses(88, 2200 - 88),
            resx(4.5, 3, 550),
            resy(5.5, 2, 250),
            phases(M_PI / 3, 0, M_PI / 6),
            freqs(1e-3, 1e3)
      {
      }

      Eigen::Vector3d interfaces;
      Eigen::Vector2d thicknesses;
      Eigen::Vector3d resx;
      Eigen::Vector3d resy;
      Eigen::Vector3d phases;
      Eigen::Vector2d freqs; // two frequencies

      double TOLERANCE = 1e-3; // TODO: tolerance too high (compare significant figures?)
    };

  TEST_F(MtAnisoScenario, ZMatrixIsCorrect)
  {
    auto Z = impedenceAniso1d(thicknesses, freqs, resx, resy, phases);

    ASSERT_EQ(2, Z.rows());
    ASSERT_EQ(4, Z.cols());

    EXPECT_NEAR(-1.1377e-09, Z(0, 0).real(), TOLERANCE);
    EXPECT_NEAR(2.36967e-05, Z(0, 0).imag(), TOLERANCE);
    EXPECT_NEAR(0.086603, Z(0, 1).real(), TOLERANCE);
    EXPECT_NEAR(0.0868367, Z(0, 1).imag(), TOLERANCE);
    EXPECT_NEAR(-0.0707112, Z(0, 2).real(), TOLERANCE);
    EXPECT_NEAR(-0.0710285, Z(0, 2).imag(), TOLERANCE);
    EXPECT_NEAR(1.1377e-09, Z(0, 3).real(), TOLERANCE);
    EXPECT_NEAR(-2.36967e-05, Z(0, 3).imag(), TOLERANCE);

    EXPECT_NEAR(4.9946, Z(1, 0).real(), TOLERANCE);
    EXPECT_NEAR(4.76612, Z(1, 0).imag(), TOLERANCE);
    EXPECT_NEAR(114.715, Z(1, 1).real(), TOLERANCE);
    EXPECT_NEAR(114.186, Z(1, 1).imag(), TOLERANCE);
    EXPECT_NEAR(-109.057, Z(1, 2).real(), TOLERANCE);
    EXPECT_NEAR(-108.497, Z(1, 2).imag(), TOLERANCE);
    EXPECT_NEAR(-4.9946, Z(1, 3).real(), TOLERANCE);
    EXPECT_NEAR(-4.76612, Z(1, 3).imag(), TOLERANCE);
  }

  TEST_F(MtAnisoScenario, phaseTensorIsCorrect)
  {
    auto Z = impedenceAniso1d(thicknesses, freqs, resx, resy, phases);
    auto tensor = phaseTensor1d(Z);

    ASSERT_EQ(2, tensor.rows());
    ASSERT_EQ(4, tensor.cols());

    EXPECT_NEAR(1.00449, tensor(0, 0), TOLERANCE);
    EXPECT_NEAR(0.000335135, tensor(0, 1), TOLERANCE);
    EXPECT_NEAR(0.000273637, tensor(0, 2), TOLERANCE);
    EXPECT_NEAR(1.0027, tensor(0, 3), TOLERANCE);
    EXPECT_NEAR(0.994941, tensor(1, 0), TOLERANCE);
    EXPECT_NEAR(-0.00188787, tensor(1, 1), TOLERANCE);
    EXPECT_NEAR(-0.00177144, tensor(1, 2), TOLERANCE);
    EXPECT_NEAR(0.995476, tensor(1, 3), TOLERANCE);
  }

  TEST_F(MtAnisoScenario, alphaBetaVectorsAreCorrect)
  {
    auto Z = impedenceAniso1d(thicknesses, freqs, resx, resy, phases);
    auto phaseTensor = phaseTensor1d(Z);
    auto alpha = alpha1d(phaseTensor);
    auto beta = beta1d(phaseTensor);

    ASSERT_EQ(2, alpha.rows());
    ASSERT_EQ(2, beta.rows());

    EXPECT_NEAR(0.164039, alpha(0), TOLERANCE);
    EXPECT_NEAR(0.71271, alpha(1), TOLERANCE);

    EXPECT_NEAR(1.53194e-05, beta(0), TOLERANCE);
    EXPECT_NEAR(-2.9249e-05, beta(1), TOLERANCE);
  }
}
}
