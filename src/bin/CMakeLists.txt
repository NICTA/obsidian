# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

SET ( shardSources
  shard.cpp)

SET ( obsidianSources obsidian.cpp )

IF(CMAKE_BUILD_TYPE MATCHES Release)

  ############################################################################
  # Obsidian Release
  ############################################################################
  ADD_EXECUTABLE (obsidian-server obsidian-server.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/app/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/comms/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/db/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/distrib/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/fwdmodel/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/infer/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/input/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/io/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/likelihood/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/prior/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/serial/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/world/blob.cpp)

  TARGET_LINK_LIBRARIES(obsidian-server
                        protobuf
                        ${LEVELDB_LIBRARY}
                        ${PROTOBUF_LIBRARIES}
                        ${ZMQ_LIBRARY}
                        ${GLOG_LIBRARY}
                        ${Boost_FILESYSTEM_LIBRARY}
                        ${Boost_SYSTEM_LIBRARY}
                        ${Boost_PROGRAM_OPTIONS_LIBRARY}
                        ${NLOPT_LIBRARIES}
                        ${STATELINE_LIBRARIES} )

  ADD_EXECUTABLE (obsidian-worker obsidian-worker.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/app/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/comms/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/db/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/distrib/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/fwdmodel/blob.cpp
                           #${OBSIDIAN_SOURCE_DIR}/src/infer/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/input/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/io/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/likelihood/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/prior/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/serial/blob.cpp
                           ${OBSIDIAN_SOURCE_DIR}/src/world/blob.cpp)

  TARGET_LINK_LIBRARIES(obsidian-worker
                        protobuf
                        ${LEVELDB_LIBRARY}
                        ${PROTOBUF_LIBRARIES}
                        ${ZMQ_LIBRARY}
                        ${GLOG_LIBRARY}
                        ${Boost_FILESYSTEM_LIBRARY}
                        ${Boost_SYSTEM_LIBRARY}
                        ${Boost_PROGRAM_OPTIONS_LIBRARY}
                        ${NLOPT_LIBRARIES}
                        ${STATELINE_LIBRARIES} )


ELSE(CMAKE_BUILD_TYPE MATCHES Release)

  ############################################################################
  # Obsidian Debug
  ############################################################################
  ADD_EXECUTABLE(obsidian-server obsidian-server.cpp ${PROTO_SRC})
  TARGET_LINK_LIBRARIES(obsidian-server ${obsidianAlgoLibraries}
                                 #${obsidianServerLibraries}
                                 ${obsidianBaseLibraries}
                                input
                                  lh
                                  distrib
                                  io
                                  )
  ADD_EXECUTABLE(obsidian-worker obsidian-worker.cpp ${PROTO_SRC})
  TARGET_LINK_LIBRARIES(obsidian-worker ${obsidianAlgoLibraries}
                                 #${obsidianServerLibraries}
                                 ${obsidianBaseLibraries}
                                input
                                  lh
                                  distrib
                                  io
                                  )
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

SET_TARGET_PROPERTIES (obsidian-server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})
SET_TARGET_PROPERTIES (obsidian-worker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})

# Macro for adding demos
FUNCTION (ADD_DEMO demoName)
  ADD_EXECUTABLE (${demoName} ${demoName}.cpp)

  SET_TARGET_PROPERTIES (${demoName} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${OBSIDIAN_BINARY_DIR})

  TARGET_LINK_LIBRARIES (${demoName} ${obsidianAlgoLibraries} ${obsidianBaseLibraries} input io serial)
ENDFUNCTION ()

ADD_CUSTOM_TARGET(symlinks ALL)

FUNCTION (SYMLINK_FILE inpath outpath)
  ADD_CUSTOM_COMMAND(TARGET symlinks COMMAND ln -fs ${inpath} ${outpath})
ENDFUNCTION ()

# Create an empty directory to hold visualisation data
FILE (MAKE_DIRECTORY ${OBSIDIAN_BINARY_DIR}/data)

## Copy over the GDF input and configuration files
SYMLINK_FILE (${OBSIDIAN_SOURCE_DIR}/src/bin/input.obsidian ${OBSIDIAN_BINARY_DIR})
