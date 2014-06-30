# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

ADD_LIBRARY(io cnpy/cnpy.cpp
               npy.cpp
               string.cpp
               csv.cpp)

TARGET_LINK_LIBRARIES(io z)
