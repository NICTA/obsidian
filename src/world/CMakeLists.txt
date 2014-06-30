# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

ADD_LIBRARY(world grid.cpp
                  interpolate.cpp
                  interpolatorspec.cpp
                  kernel.cpp
                  property.cpp
                  transitions.cpp
                  voxelise.cpp)

ADD_EXECUTABLE(test-world testworld.cpp)
TARGET_LINK_LIBRARIES(test-world world ${obsidianBaseLibraries})
REGISTER_UNIT_TESTS(test-world)

ADD_EXECUTABLE(test-boundaries-are-times testboundariesaretimes.cpp)
TARGET_LINK_LIBRARIES(test-boundaries-are-times ${obsidianAlgoLibraries} ${obsidianBaseLibraries})
REGISTER_UNIT_TESTS(test-boundaries-are-times)
