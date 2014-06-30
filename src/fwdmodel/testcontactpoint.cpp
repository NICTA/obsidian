//!
//! Test contact point 1d sensor forward model.
//!
//! \file fwdmodel/testcontactpoint.cpp
//! \author Nahid Akbar
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <gtest/gtest.h>

#include "datatype/datatypes.hpp"
#include "world/interpolate.hpp"
#include "fwdmodel/fwd.hpp"
#include "test/world.hpp"

namespace obsidian
{
  class SensorTest: public ::testing::Test
  {
  public:
    std::vector<WorldSpec> worldSpecs;
    std::vector<WorldParams> worldParams;
    std::vector<ContactPointSpec> sensorSpec;
    std::vector<ContactPointParams> sensorParams;
    std::vector<ContactPointResults> sensorResults;

    virtual void SetUp()
    {
      WorldSpec worldSpec_;
      WorldParams worldParams_;

      testing::initWorld(worldSpec_, worldParams_, -10, 10, 20, -10, 10, 20, 0, 20, 5, [](double x, double y, uint boundary)
      {
        return boundary;
      },
                         [](double x, double y, double boundary)
                         {
                           return 0;
                         },
                         [](uint layer, uint property)
                         {
                           if (property == static_cast<uint>(RockProperty::PWaveVelocity))
                           {
                             return 1.0;
                           }
                           return 0.0;
                         });

      worldSpecs.push_back(worldSpec_);
      worldParams.push_back(worldParams_);

      ContactPointSpec spec;
      spec.locations.resize(2, 3);
      for (uint sensor = 0; sensor < 2; sensor++)
      {
        spec.locations(sensor, 0) = sensor * sin(sensor / 5.0 * 2.0 * M_PI);
        spec.locations(sensor, 1) = sensor * cos(sensor / 5.0 * 2.0 * M_PI);
        spec.locations(sensor, 2) = 0;
        Eigen::VectorXi interface(4);
        for (uint boundary = 0; boundary < 4; boundary++)
        {
          interface(boundary) = boundary;
        }
        spec.interfaces.push_back(interface);
      }
      sensorSpec.push_back(spec);
      ContactPointParams params;
      params.returnSensorData = true;
      sensorParams.push_back(params);
      ContactPointResults results;
      Eigen::VectorXd readings(4);
      readings << 0, 1, 2, 3;
      results.readings.push_back(readings);
      results.readings.push_back(readings);
      sensorResults.push_back(results);
    }
  };

TEST_F(SensorTest, test)
{
  for (size_t world = 0; world < worldSpecs.size(); world++)
  {
    WorldSpec worldSpec = worldSpecs[world];
    std::vector<world::InterpolatorSpec> layerInterp = world::worldspec2Interp(worldSpec);

    WorldParams worldParam = worldParams[world];

    for (uint sensor = 0; sensor < sensorSpec.size(); sensor++)
    {
      ContactPointSpec spec = sensorSpec[sensor];
      ContactPointCache cache = fwd::generateCache<ForwardModel::CONTACTPOINT>(layerInterp, worldSpec, spec);
      ContactPointResults results = fwd::forwardModel<ForwardModel::CONTACTPOINT>(spec, cache, worldParam);
      ContactPointResults expected = sensorResults[sensor];
      double error = 0;
      for (uint r = 0; r < results.readings.size(); r++)
      {
        error += (results.readings[r] - expected.readings[r]).norm();
      }
      EXPECT_LT(error, 0.1);
    }
  }
}
}

int main(int argc, char **argv)
{
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}
