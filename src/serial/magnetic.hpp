//!
//! Magnetics Forward Model serialisation.
//!
//! \file serial/magnetic.hpp
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
    std::string serialise(const MagSpec& g);
    std::string serialise(const MagParams& g);
    std::string serialise(const MagResults& g);

    void unserialise(const std::string& s, MagParams& g);
    void unserialise(const std::string& s, MagSpec& g);
    void unserialise(const std::string& s, MagResults& g);
  }
}
