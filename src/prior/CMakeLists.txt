# Copyright (c) 2014, NICTA. 
# Affero General Public License version 3 or later
# See the COPYRIGHT file.

# Authors: Lachlan McCalman
# Date: 2014 

ADD_LIBRARY(prior  prior.cpp
                   world.cpp)

#ADD_EXECUTABLE ( test-prior  "testprior.cpp")

#TARGET_LINK_LIBRARIES ( test-prior
#  console
#  world
#  prior
#  ${GLOG_LIBRARY}
#  ${Boost_LIBRARIES}
#  gtest)

#REGISTER_UNIT_TESTS( test-prior)
