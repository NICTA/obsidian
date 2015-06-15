/**
 * Contains the basic obsidian datatypes for communication between forward models.
 *
 * @file testseismic.hpp
 * @author Nahid Akbar
 * @date 2014
 * @license General Public License version 3 or later.
 * @copyright (c) 2014, NICTA
 */

#include "serial/seismic.hpp"
#include "test/seismic.hpp"
#include "test/serial.hpp"

namespace obsidian
{

TEST_F(Serialise, testSpec)
{
  generateVariations<Seismic1dSpec>(test<Seismic1dSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<Seismic1dParams>(test<Seismic1dParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<Seismic1dResults>(test<Seismic1dResults>);
}

}
