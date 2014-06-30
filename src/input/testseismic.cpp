/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testseismic.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "test/seismic.hpp"
#include "test/input.hpp"
#include "input.hpp"

namespace obsidian
{

TEST_F(InputTest, testSpec)
{
  generateVariations<Seismic1dSpec>(testSpecCSV<ForwardModel::SEISMIC1D>);
}

TEST_F(InputTest, testResult)
{
  generateVariations<Seismic1dResults>(testResultsCSV<ForwardModel::SEISMIC1D>);
}

}
