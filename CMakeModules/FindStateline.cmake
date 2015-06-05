# Copyright (c) 2013, NICTA. 
# This file is licensed under the General Public License version 3 or later.
# See the COPYRIGHT file.

# Find STATELINE
#
#  Env variable used as hint for finders:
#  STATELINE_ROOT_DIR:            Base directory where all STATELINE components are found
#
# The following are set after configuration is done: 
#  STATELINE_FOUND
#  STATELINE_INCLUDE_DIR
#  STATELINE_LIBRARIES

include(FindPackageHandleStandardArgs)

find_path(STATELINE_INCLUDE_DIR stateline/app/workerwrapper.hpp
  HINTS
  ${STATELINE_ROOT_DIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(STATELINE_CLIENT_LIBRARY
  NAMES statelineclient
  HINTS
  ${STATELINE_ROOT_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  )

FIND_LIBRARY(STATELINE_SERVER_LIBRARY
  NAMES statelineserver
  HINTS
  ${STATELINE_ROOT_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  )

find_package_handle_standard_args(STATELINE DEFAULT_MSG
                                  STATELINE_INCLUDE_DIR
                                  STATELINE_CLIENT_LIBRARY
                                  STATELINE_SERVER_LIBRARY)

if(STATELINE_FOUND)
    set(STATELINE_LIBRARIES ${STATELINE_SERVER_LIBRARY} ${STATELINE_CLIENT_LIBRARY})
endif()

#MARK_AS_ADVANCED(STATELINE_INCLUDE_DIR STATELINE_CLIENT_LIBRARY STATELINE_SERVER_LIBRARY STATELINE_LIBRARIES)