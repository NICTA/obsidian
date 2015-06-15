/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testcontactpoint.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "test/world.hpp"
#include "test/input.hpp"
#include "input/input.hpp"
#include <functional>

namespace obsidian
{

  TEST_F(InputTest, testSpec)
  {
    auto fn = std::bind(testCSV<WorldSpec>, std::placeholders::_1, initWorldOptions, parseSpec<WorldSpec>, "world");
    generateVariations<WorldSpec>(fn);
  }

  TEST_F(InputTest, testParam)
  {
    auto fn = std::bind(testCSV<WorldParams>, std::placeholders::_1, initWorldOptions, parseSimulationParams<WorldParams>, "world");
    generateVariations<WorldParams>(fn);
  }

  void initOptionsHack(po::options_description & od)
  {
    initWorldOptions(od);
    initSensorInputFileOptions<ForwardModel::MTANISO>(od);
  }

TEST_F(InputTest, testPrior)
{
  auto fn = std::bind(testCSV<prior::WorldParamsPrior>, std::placeholders::_1, initOptionsHack, parsePrior < prior::WorldParamsPrior > , "world");
  generateVariations<prior::WorldParamsPrior>(fn);
}

}
