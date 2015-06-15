// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

#include <glog/logging.h>
#include "gtest/gtest.h"

#include "datatype/datatypes.hpp"
#include "prior/prior.hpp"
#include "test/world.hpp"

#include <functional>
#include <memory>
#include <iostream>
namespace ph = std::placeholders;

namespace obsidian
{

  template<typename T>
  bool operator==(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& g, const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& p)
  {
    return (g.rows() == p.rows()) && (g.cols() == p.cols()) && (g - p).norm() < 1e-3;
  }

  template<typename T>
  bool operator==(const Eigen::Matrix<T, Eigen::Dynamic, 1>& g, const Eigen::Matrix<T, Eigen::Dynamic, 1>& p)
  {
    return (g.size() == p.size()) && (g - p).norm() < 1e-3;
  }

  namespace prior
  {
    class PriorTest: public ::testing::Test
    {
    public:

      WorldSpec worldSpec;
      std::vector<distrib::MultiGaussian> geomPrior;
      std::unique_ptr<prior::WorldParamsPrior> ptrPrior;

      WorldParams testParams;
      Eigen::VectorXd testTheta;

      PriorTest()
          : worldSpec(0, 50000, 0, 50000, 0, 5000), testTheta(94)
      {
        // WorldSpec
        std::vector<Eigen::MatrixXi> ctrlPointMasks;
        std::vector<Eigen::MatrixXd> ctrlPointMins;
        std::vector<Eigen::MatrixXd> ctrlPointMaxs;

        std::vector<Eigen::VectorXi> propMasks;
        std::vector<Eigen::VectorXd> propMins;
        std::vector<Eigen::VectorXd> propMaxs;
        worldSpec.boundaries.push_back( { Eigen::MatrixXd::Ones(200, 200) * 0, std::make_pair(1, 1), BoundaryClass::Normal });
        worldSpec.boundaries.push_back( { Eigen::MatrixXd::Ones(200, 200) * 750, std::make_pair(3, 3), BoundaryClass::Normal });
        worldSpec.boundaries.push_back( { Eigen::MatrixXd::Ones(200, 200) * 1500, std::make_pair(3, 3), BoundaryClass::Normal });
        worldSpec.boundaries.push_back( { Eigen::MatrixXd::Ones(200, 200) * 1200, std::make_pair(7, 7), BoundaryClass::Warped });

        // "the drawn parameters...
        testParams.controlPoints.push_back(Eigen::MatrixXd::Ones(1, 1) * 3.0);
        testParams.controlPoints.push_back(Eigen::MatrixXd::Identity(3, 3) * 6.0);
        testParams.controlPoints.push_back(Eigen::MatrixXd::Ones(3, 3) * 9.0);
        testParams.controlPoints.push_back(Eigen::MatrixXd::Ones(7, 7) * 12.0);

        ctrlPointMasks.push_back(Eigen::MatrixXi::Ones(1, 1));
        ctrlPointMasks.push_back(Eigen::MatrixXi::Identity(3, 3));
        ctrlPointMasks.push_back(Eigen::MatrixXi::Ones(3, 3));
        ctrlPointMasks.push_back(Eigen::MatrixXi::Ones(7, 7));

        ctrlPointMins.push_back(Eigen::MatrixXd::Ones(1, 1) * -std::numeric_limits<double>::max() / 5);
        ctrlPointMins.push_back(Eigen::MatrixXd::Ones(3, 3) * -std::numeric_limits<double>::max() / 5);
        ctrlPointMins.push_back(Eigen::MatrixXd::Ones(3, 3) * -std::numeric_limits<double>::max() / 5);
        ctrlPointMins.push_back(Eigen::MatrixXd::Ones(7, 7) * -std::numeric_limits<double>::max() / 5);

        ctrlPointMaxs.push_back(Eigen::MatrixXd::Ones(1, 1) * std::numeric_limits<double>::max() / 5);
        ctrlPointMaxs.push_back(Eigen::MatrixXd::Ones(3, 3) * std::numeric_limits<double>::max() / 5);
        ctrlPointMaxs.push_back(Eigen::MatrixXd::Ones(3, 3) * std::numeric_limits<double>::max() / 5);
        ctrlPointMaxs.push_back(Eigen::MatrixXd::Ones(7, 7) * std::numeric_limits<double>::max() / 5);

        Eigen::VectorXd coupledSds = Eigen::VectorXd::Zero(4);
        Eigen::VectorXd uncoupledSds = Eigen::VectorXd::Zero(4);

        uint nProps = (uint) RockProperty::Count;
        Eigen::VectorXi pmask(nProps);
        pmask << 0, 0, 1, 0, 1, 1, 0, 1, 1;
        Eigen::VectorXd pmin = Eigen::VectorXd::Zero(nProps);
        Eigen::VectorXd pmax = Eigen::VectorXd::Ones(nProps) * 9e99;
        propMasks.push_back(Eigen::VectorXi::Ones(nProps));
        propMasks.push_back(Eigen::VectorXi::Ones(nProps));
        propMasks.push_back(pmask);
        propMasks.push_back(Eigen::VectorXi::Ones(nProps));
        propMins.push_back(pmin);
        propMins.push_back(pmin);
        propMins.push_back(pmin);
        propMins.push_back(pmin);
        propMaxs.push_back(pmax);
        propMaxs.push_back(pmax);
        propMaxs.push_back(pmax);
        propMaxs.push_back(pmax);
        testParams.rockProperties.push_back(Eigen::VectorXd::Ones(nProps) * 0.1);
        testParams.rockProperties.push_back(Eigen::VectorXd::Ones(nProps) * 0.2);
        Eigen::VectorXd layer3(nProps);
        layer3 << 2.0, 2.0, 0.3, 2.0, 0.3, 0.3, 2.0, 0.3, 0.3;
        testParams.rockProperties.push_back(layer3);
        testParams.rockProperties.push_back(Eigen::VectorXd::Ones(nProps) * 0.4);

        testTheta << -1.9, -1.9, -1.9, -1.9, -1.9, -1.9, -1.9, -1.9, -1.9, -1.8, -1.8, -1.8, -1.8, -1.8, -1.8, -1.8, -1.8, -1.8, -1.7, -1.7, -1.7, -1.7, -1.7, -1.6, -1.6, -1.6, -1.6, -1.6, -1.6, -1.6, -1.6, -1.6, 3, 0.0212132, 0.0212132, 0.0212132, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.0318198, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012, 0.012;

        // Rock Properties
        std::vector<distrib::MultiGaussian> rockPropertyPrior;
        rockPropertyPrior.push_back(distrib::MultiGaussian(2.0 * Eigen::VectorXd::Ones(nProps), 1 * Eigen::MatrixXd::Ones(nProps, nProps)));
        rockPropertyPrior.push_back(distrib::MultiGaussian(2.0 * Eigen::VectorXd::Ones(nProps), 1 * Eigen::MatrixXd::Ones(nProps, nProps)));
        rockPropertyPrior.push_back(distrib::MultiGaussian(2.0 * Eigen::VectorXd::Ones(nProps), 1 * Eigen::MatrixXd::Ones(nProps, nProps)));
        rockPropertyPrior.push_back(distrib::MultiGaussian(2.0 * Eigen::VectorXd::Ones(nProps), 1 * Eigen::MatrixXd::Ones(nProps, nProps)));
        // Control Points
        std::vector<distrib::MultiGaussian> ctrlPointPrior;
        ctrlPointPrior.push_back(distrib::coupledGaussianBlock(Eigen::MatrixXd::Zero(1, 1), 0, 1));
        ctrlPointPrior.push_back(distrib::coupledGaussianBlock(Eigen::MatrixXd::Zero(3, 3), 200, 200));
        ctrlPointPrior.push_back(distrib::coupledGaussianBlock(Eigen::MatrixXd::Zero(3, 3), 200, 200));
        ctrlPointPrior.push_back(distrib::coupledGaussianBlock(Eigen::MatrixXd::Zero(7, 7), 0, 1000));
        // Classes
        std::vector<BoundaryClass> classes;
        classes.push_back(BoundaryClass::Normal);
        classes.push_back(BoundaryClass::Normal);
        classes.push_back(BoundaryClass::Normal);
        classes.push_back(BoundaryClass::Warped);

        // Create the prior
        ptrPrior.reset(
            new prior::WorldParamsPrior(ctrlPointPrior, ctrlPointMasks, ctrlPointMins, ctrlPointMaxs, coupledSds, uncoupledSds,
                                        rockPropertyPrior, propMasks, propMins, propMaxs, classes));
      }
    };

  TEST_F(PriorTest, deconstruct)
  {
    Eigen::VectorXd theta = ptrPrior->deconstruct(testParams);
//      Eigen::Matrix<double, -1, -1, Eigen::ColMajor> matrix(testTheta.rows(), 2);
//      matrix.col(0) = theta;
//      matrix.col(1) = testTheta;
//      std::cout << matrix << std::endl;
    double error = (theta - testTheta).norm();
//      std::cout << error << std::endl;
//      std::cout << testTheta << std::endl;
//      EXPECT_EQ(theta, testTheta);
    EXPECT_LT(error, 1e-3);
  }

  TEST_F(PriorTest, reconstruct)
  {
    WorldParams params = ptrPrior->reconstruct(testTheta);
    EXPECT_EQ(testParams, params);
  }

  TEST_F(PriorTest, sample)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    Eigen::VectorXd theta1 = ptrPrior->sample(gen);
    WorldParams worldParams1 = ptrPrior->reconstruct(theta1);
    Eigen::VectorXd theta2 = ptrPrior->deconstruct(worldParams1);
    WorldParams worldParams2 = ptrPrior->reconstruct(theta2);
    EXPECT_EQ(worldParams1, worldParams2);
  }

}
 // namespace obsidian
}// namespace prior
