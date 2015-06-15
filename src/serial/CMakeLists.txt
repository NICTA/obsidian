# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

set(PROTOBUF_FILES "serialtypes.proto")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTOBUF_FILES})

ADD_LIBRARY(protobuf ${PROTO_SRC}
                     ${PROTO_HEADER})

ADD_LIBRARY(serial gravity.cpp magnetic.cpp  mt.cpp
                   utility.cpp  world.cpp seismic.cpp
                   contactpoint.cpp thermal.cpp)

ADD_DEPENDENCIES(serial protobuf)
                    
TARGET_LINK_LIBRARIES(serial world protobuf ${PROTOBUF_LIBRARIES})

ADD_SUBDIRECTORY(test)
