//!
//! Seismic Forward Model serialisation.
//!
//! \file serial/seismic.hpp
//! \author Nahid Akbar
//! \date May, 2014
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
    std::string serialise(const Seismic1dSpec& g);
    std::string serialise(const Seismic1dParams& g);
    std::string serialise(const Seismic1dResults& g);

    void unserialise(const std::string& s, Seismic1dSpec& g);
    void unserialise(const std::string& s, Seismic1dParams& g);
    void unserialise(const std::string& s, Seismic1dResults& g);
  }
}
