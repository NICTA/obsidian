//! Test seismic 1d sensor forward model.
//!
//! \file fwdmodel/testseismic.cpp
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
    std::vector<Seismic1dSpec> sensorSpec;
    std::vector<Seismic1dParams> sensorParams;
    std::vector<Seismic1dResults> sensorResults;

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

      Seismic1dSpec spec;
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
      Seismic1dParams params;
      params.returnSensorData = true;
      sensorParams.push_back(params);
      Seismic1dResults results;
      Eigen::VectorXd readings(4);
      readings << 2, 4, 6, 8;
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
    std::vector<world::InterpolatorSpec> boundaryInterp = world::worldspec2Interp(worldSpec);

    WorldParams worldParam = worldParams[world];
    //    std::cout << worldSpec;
    //    std::cout << worldParam;

    for (uint sensor = 0; sensor < sensorSpec.size(); sensor++)
    {
      Seismic1dSpec spec = sensorSpec[sensor];
      Seismic1dCache cache = fwd::generateCache<ForwardModel::SEISMIC1D>(boundaryInterp, worldSpec, spec);
      Seismic1dResults results = fwd::forwardModel<ForwardModel::SEISMIC1D>(spec, cache, worldParam);
      Seismic1dResults expected = sensorResults[sensor];
      //      std::cout << spec;
      //      std::cout << results;
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

