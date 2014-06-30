//!
//! MT Forward Model serialisation.
//!
//! \file serial/mt.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/datatypes.hpp"
#include "serial/utility.hpp"

#include <string>

namespace obsidian
{
  namespace comms
  {
    std::string serialise(const MtAnisoSpec& g);
    void unserialise(const std::string& s, MtAnisoSpec& g);

    std::string serialise(const MtAnisoParams& g);
    void unserialise(const std::string& s, MtAnisoParams& g);

    std::string serialise(const MtAnisoResults& g);
    void unserialise(const std::string& s, MtAnisoResults& g);
  }
}
