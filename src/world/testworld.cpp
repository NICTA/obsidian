//!
//! Contains tests for world model.
//!
//! \file world/testworld.cpp
//! \author Nahid Akbar
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <gtest/gtest.h>

#include "app/console.hpp"

namespace obsidian
{
}

const int logLevel = -3;
const bool stdErr = false;
std::string directory = ".";

int main (int ac, char** av)
{
  obsidian::init::initialiseLogging("testworld", logLevel, stdErr, directory);
  testing::InitGoogleTest(&ac, av);
  auto result = RUN_ALL_TESTS();
  return result;
}
