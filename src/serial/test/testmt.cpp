//! MT Test suite
//! 
//! @file testseismic.hpp
//! @author Nahid Akbar
//! @date 2014
//! @license Affero General Public License version 3 or later.
//! @copyright (c) 2014, NICTA

#include "serial/mt.hpp"
#include "test/mt.hpp"
#include "test/serial.hpp"

namespace obsidian
{

TEST_F(Serialise, testSpec)
{
  generateVariations<MtAnisoSpec>(test<MtAnisoSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<MtAnisoParams>(test<MtAnisoParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<MtAnisoResults>(test<MtAnisoResults>);
}

}
