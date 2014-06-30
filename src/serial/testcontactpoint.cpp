// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testcontactpoint.hpp
 * @author Nahid Akbar
 * @date 2014
 */

#include "serial/contactpoint.hpp"
#include "test/serial.hpp"
#include "test/contactpoint.hpp"

namespace obsidian
{
TEST_F(Serialise, testSpec)
{
  generateVariations<ContactPointSpec>(test<ContactPointSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<ContactPointParams>(test<ContactPointParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<ContactPointResults>(test<ContactPointResults>);
}
}
