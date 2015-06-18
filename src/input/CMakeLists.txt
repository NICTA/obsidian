# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

ADD_LIBRARY(input input.cpp
                  global.cpp
                  world.cpp
                  gravity.cpp
                  magnetism.cpp
                  mt.cpp
                  seismic.cpp
                  contactpoint.cpp
                  thermal.cpp)

target_link_libraries(input fwd-mt world ${Boost_LIBRARIES})

ADD_SUBDIRECTORY(test)
