/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testseismic.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "serial/world.hpp"
#include "test/world.hpp"
#include "test/serial.hpp"

namespace obsidian
{

TEST_F(Serialise, testSpec)
{
  generateVariations<WorldSpec>(test<WorldSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<WorldParams>(test<WorldParams>);
}

}
