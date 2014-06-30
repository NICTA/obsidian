//!
//! Serialisation of infer/mcmctypes.hpp types.
//!
//! \file serial/mcmc.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "infer/mcmctypes.hpp"
#include "serial/utility.hpp"
#include <string>

namespace stateline
{
  namespace comms
  {
    std::string serialise(const mcmc::State& g);
    void unserialise(const std::string& s, mcmc::State& g);
  }
}
