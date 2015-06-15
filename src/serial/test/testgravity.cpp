// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testgravity.hpp
 * @author Nahid Akbar
 * @date 2014
 */

#include "serial/gravity.hpp"
#include "test/serial.hpp"
#include "test/gravity.hpp"

namespace obsidian
{
TEST_F(Serialise, testSpec)
{
  generateVariations<GravSpec>(test<GravSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<GravParams>(test<GravParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<GravResults>(test<GravResults>);
}

}
