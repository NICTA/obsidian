//!
//! Thermal Forward Model serialisation.
//!
//! \file serial/thermal.hpp
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
    std::string serialise(const ThermalSpec& g);
    void unserialise(const std::string& s, ThermalSpec& g);

    std::string serialise(const ThermalParams& g);
    void unserialise(const std::string& s, ThermalParams& g);

    std::string serialise(const ThermalResults& g);
    void unserialise(const std::string& s, ThermalResults& g);
  }
}

