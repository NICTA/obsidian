//!
//! World Model serialisation.
//!
//! \file serial/world.hpp
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
    // message WorldParamsProtobuf
    // {
    // repeated bytes rockProperties = 1[packed=true];
    // repeated bytes controlPoints = 2[packed=true];
    // }

    std::string serialise(const WorldSpec& g);
    std::string serialise(const WorldParams& g);

    void unserialise(const std::string& s, WorldParams& g);
    void unserialise(const std::string& s, WorldSpec& g);

  }
}
