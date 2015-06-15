// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * Contains the basic obsidian datatypes for communication between forward models.
 *
 * @file testmagnetism.hpp
 * @author Nahid Akbar
 * @date 2014
 */

#include "serial/magnetic.hpp"
#include "test/serial.hpp"
#include "test/magnetism.hpp"

namespace obsidian
{
TEST_F(Serialise, testSpec)
{
  generateVariations<MagSpec>(test<MagSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<MagParams>(test<MagParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<MagResults>(test<MagResults>);
}

}
