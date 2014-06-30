# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

set(PROTOBUF_FILES "serialtypes.proto" "stateline.proto")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTOBUF_FILES})

ADD_LIBRARY(protobuf ${PROTO_SRC}
                     ${PROTO_HEADER})

ADD_LIBRARY(serial   gravity.cpp 
   magnetic.cpp  mt.cpp  utility.cpp  world.cpp
   commstypes.cpp seismic.cpp contactpoint.cpp thermal.cpp  mcmc.cpp)

ADD_EXECUTABLE ( test-serial-gravity testgravity.cpp)
TARGET_LINK_LIBRARIES(test-serial-gravity serial)
REGISTER_UNIT_TESTS( test-serial-gravity )

ADD_EXECUTABLE ( test-serial-magnetism testmagnetism.cpp)
TARGET_LINK_LIBRARIES(test-serial-magnetism serial)
REGISTER_UNIT_TESTS( test-serial-magnetism )

ADD_EXECUTABLE ( test-serial-mt testmt.cpp)
TARGET_LINK_LIBRARIES(test-serial-mt serial)
REGISTER_UNIT_TESTS( test-serial-mt )

ADD_EXECUTABLE ( test-serial-seismic testseismic.cpp)
TARGET_LINK_LIBRARIES(test-serial-seismic serial)
REGISTER_UNIT_TESTS( test-serial-seismic )

ADD_EXECUTABLE ( test-serial-contactpoint testcontactpoint.cpp)
TARGET_LINK_LIBRARIES(test-serial-contactpoint serial)
REGISTER_UNIT_TESTS( test-serial-contactpoint )

ADD_EXECUTABLE ( test-serial-world testworld.cpp)
TARGET_LINK_LIBRARIES(test-serial-world serial)
REGISTER_UNIT_TESTS( test-serial-world )

ADD_EXECUTABLE ( test-serial-thermal testthermal.cpp)
TARGET_LINK_LIBRARIES(test-serial-thermal serial)
REGISTER_UNIT_TESTS( test-serial-thermal )

ADD_DEPENDENCIES(serial protobuf)
                    
TARGET_LINK_LIBRARIES(serial world protobuf
  ${PROTOBUF_LIBRARIES})
