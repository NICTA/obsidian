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

ADD_SUBDIRECTORY(test)
