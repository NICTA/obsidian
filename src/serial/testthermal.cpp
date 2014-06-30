// Copyright (c) 2014, NICTA.
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * Contains the basic GDF datatypes for communication between forward models.
 *
 * @file testthermal.hpp
 * @author Nahid Akbar
 * @date 2014
 */

#include "serial/thermal.hpp"
#include "test/serial.hpp"
#include "test/thermal.hpp"

namespace obsidian
{
TEST_F(Serialise, testSpec)
{
  generateVariations<ThermalSpec>(test<ThermalSpec>);
}

TEST_F(Serialise, testParams)
{
  generateVariations<ThermalParams>(test<ThermalParams>);
}

TEST_F(Serialise, testResults)
{
  generateVariations<ThermalResults>(test<ThermalResults>);
}

}
