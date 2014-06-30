//!
//! Gravity Forward Model serialisation.
//!
//! \file serial/gravity.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/datatypes.hpp"

#include <string>

namespace obsidian
{
  namespace comms
  {
    std::string serialise(const GravSpec& g);
    void unserialise(const std::string& s, GravSpec& g);

    std::string serialise(const GravParams& g);
    void unserialise(const std::string& s, GravParams& g);

    std::string serialise(const GravResults& g);
    void unserialise(const std::string& s, GravResults& g);
  }
}

