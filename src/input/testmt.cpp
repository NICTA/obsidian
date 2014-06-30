/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testseismic.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "test/input.hpp"
#include "input.hpp"
#include "test/mt.hpp"

namespace obsidian
{

TEST_F(InputTest, testSpec)
{
  generateVariations<MtAnisoSpec>(testSpecCSV<ForwardModel::MTANISO>);
}

TEST_F(InputTest, testResult)
{
  generateVariations<MtAnisoResults>(testResultsCSV<ForwardModel::MTANISO>);
}

}
