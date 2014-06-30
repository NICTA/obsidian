/**
 * Test Seismic 1d sensor forward model.
 *
 * @file testseismic.hpp
 * @author Nahid Akbar
 * @date 2014-05-27
 * @copyright (c) 2014, NICTA
 * @license General Public License version 3 or later
 */

#include "input.hpp"
#include "datatype/sensors.hpp"

#include <gtest/gtest.h>

#include "test/common.hpp"

namespace obsidian
{

  class InputTest: public ::testing::Test
  {
  public:
  };

  template<ForwardModel f>
  struct testSensorPropertiesMask
  {
    testSensorPropertiesMask(Eigen::VectorXi & propMasksMask)
    {
      enableProperties<f>(propMasksMask);
    }
  };
TEST_F(InputTest, inputMaskTest)
{
  Eigen::VectorXi properties (static_cast<uint>(RockProperty::Count));

  applyToSensors<testSensorPropertiesMask>(properties);
}

}

int main(int argc, char **argv)
{
::testing::InitGoogleTest(&argc, argv);
return RUN_ALL_TESTS();
}

