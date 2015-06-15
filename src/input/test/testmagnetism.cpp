/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testcontactpoint.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "test/magnetism.hpp"
#include "test/input.hpp"
#include "input/input.hpp"

namespace obsidian
{

TEST_F(InputTest, testSpec)
{
  generateVariations<MagSpec>(testSpecCSV<ForwardModel::MAGNETICS>);
}

TEST_F(InputTest, testResult)
{
  generateVariations<MagResults>(testResultsCSV<ForwardModel::MAGNETICS>);
}

}
