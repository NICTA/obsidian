# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

# Gravity and magnetic forward model
ADD_LIBRARY(fwd-gravmag gravmag.cpp
                        gravity.cpp
                        magnetic.cpp)

# MT forward model
ADD_LIBRARY(fwd-mt mt1d.cpp)
target_link_libraries(fwd-mt world)

# Seismic forward model
ADD_LIBRARY(fwd-seismic seismic.cpp)

# Contact point forward model
ADD_LIBRARY(fwd-contactpoint contactpoint.cpp)

# Thermal forward model
ADD_LIBRARY(fwd-thermal thermal.cpp)

ADD_SUBDIRECTORY(test)
