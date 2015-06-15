# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

# Gravity and magnetic forward model
ADD_LIBRARY(fwd-gravmag gravmag.cpp
                        gravity.cpp
                        magnetic.cpp)

SET(TEST_LIBS world
              console
              ${GLOG_LIBRARY}
              ${Boost_PROGRAM_OPTIONS_LIBRARY})

ADD_EXECUTABLE(test-fwd-gravmag testgravmag.cpp)
TARGET_LINK_LIBRARIES(test-fwd-gravmag ${TEST_LIBS})
REGISTER_UNIT_TESTS(test-fwd-gravmag)

# MT forward model
ADD_LIBRARY(fwd-mt mt1d.cpp)

ADD_EXECUTABLE(test-fwd-mt test1dmt.cpp)
TARGET_LINK_LIBRARIES(test-fwd-mt fwd-mt ${TEST_LIBS})
REGISTER_UNIT_TESTS(test-fwd-mt)

# Seismic forward model
ADD_LIBRARY(fwd-seismic seismic.cpp)

ADD_EXECUTABLE(test-fwd-seismic testseismic.cpp)
TARGET_LINK_LIBRARIES(test-fwd-seismic fwd-seismic ${TEST_LIBS})
REGISTER_UNIT_TESTS(test-fwd-seismic)

# Contact point forward model
ADD_LIBRARY(fwd-contactpoint contactpoint.cpp)

ADD_EXECUTABLE(test-fwd-contactpoint testcontactpoint.cpp)
TARGET_LINK_LIBRARIES(test-fwd-contactpoint fwd-contactpoint ${TEST_LIBS})
REGISTER_UNIT_TESTS(test-fwd-contactpoint)

# Thermal forward model
ADD_LIBRARY(fwd-thermal thermal.cpp)

ADD_EXECUTABLE(test-fwd-thermal testtherm.cpp)
TARGET_LINK_LIBRARIES(test-fwd-thermal ${TEST_LIBS})
REGISTER_UNIT_TESTS(test-fwd-thermal)
