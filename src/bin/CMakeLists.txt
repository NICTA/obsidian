# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

SET(obsidian3rdPartyLibs
        ${ZMQ_LIBRARY}
        ${GLOG_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${STATELINE_LIBRARIES})

SET(obsidianLibs
        input lh distrib io
        world prior serial
        fwd-gravmag fwd-mt fwd-seismic
        fwd-contactpoint fwd-thermal)

ADD_EXECUTABLE(obsidian-server obsidian-server.cpp ${PROTO_SRC})
TARGET_LINK_LIBRARIES(obsidian-server ${GLOG_LIBRARY}
                                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                                      ${STATELINE_LIBRARIES})

ADD_EXECUTABLE(obsidian-worker obsidian-worker.cpp ${PROTO_SRC})
TARGET_LINK_LIBRARIES(obsidian-worker ${obsidianLibs} ${obsidian3rdPartyLibs})

SET_TARGET_PROPERTIES(obsidian-server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})
SET_TARGET_PROPERTIES(obsidian-worker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})

# Macro for adding demos
FUNCTION(ADD_DEMO demoName)
  ADD_EXECUTABLE(${demoName} ${demoName}.cpp)

  SET_TARGET_PROPERTIES(${demoName} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})

  TARGET_LINK_LIBRARIES(${demoName} ${obsidianLibs} ${obsidian3rdPartyLibs} input io serial)
ENDFUNCTION()

ADD_CUSTOM_TARGET(symlinks ALL)

FUNCTION(SYMLINK_FILE inpath outpath)
  ADD_CUSTOM_COMMAND(TARGET symlinks COMMAND ln -fs ${inpath} ${outpath})
ENDFUNCTION()

# Create an empty directory to hold visualisation data
FILE(MAKE_DIRECTORY ${OBSIDIAN_BINARY_DIR}/data)

# Copy over the GDF input and configuration files
SYMLINK_FILE(${OBSIDIAN_SOURCE_DIR}/src/bin/input.obsidian ${OBSIDIAN_BINARY_DIR})
