//!
//! Contains tests for using waves times instead of depth to define boundaries.
//!
//! \file world/testboundariesaretimes.cpp
//! \author Nahid Akbar
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <gtest/gtest.h>

#include "app/console.hpp"
#include "test/world.hpp"

#include "datatype/datatypes.hpp"
#include "world/interpolate.hpp"
#include "world/transitions.hpp"

namespace obsidian
{
  class WorldTest: public ::testing::Test
  {
  };

  TEST_F(WorldTest, test)
  {
    WorldSpec spec;
    WorldParams params;

    testing::initWorld(spec, params, -10, 10, 20, -10, 10, 20, 0,
        20, 5, [](double x, double y, uint boundary)
        {
        return boundary;
        }, [](double x, double y, double boundary)
        {
        return 0;
        }, [](uint layer, uint property)
        {
        if (property == static_cast<uint>(RockProperty::PWaveVelocity))
        {
        return 1.0 + (layer % 2);
        }
        return 0.0;
        });
    spec.boundariesAreTimes = true;
    std::vector<world::InterpolatorSpec> interpolation = world::worldspec2Interp(spec);
    Eigen::MatrixXd locations = Eigen::MatrixXd::Zero(1, 2);
    world::Query query(interpolation, spec, locations);
    Eigen::MatrixXd transitions = world::getTransitions(interpolation, params, query); // mqueries (mlayer) x nlocations
    Eigen::MatrixXd expected_transitions (5, 1);
    expected_transitions << 0, 1, 5, 8, 16;
    double error = (transitions - expected_transitions).norm();
    EXPECT_LT(error, 0.01);
  }
}

const int logLevel = -3;
const bool stdErr = false;
std::string directory = ".";

int main (int ac, char** av)
{
  obsidian::init::initialiseLogging("testboundariesaretimes", logLevel, stdErr, directory);
  testing::InitGoogleTest(&ac, av);
  auto result = RUN_ALL_TESTS();
  return result;
}
