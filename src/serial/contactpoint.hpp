//!
//! Contact Point Forward Model serialisation.
//!
//! \file serial/contactpoint.hpp
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
    std::string serialise(const ContactPointSpec& g);
    std::string serialise(const ContactPointParams& g);
    std::string serialise(const ContactPointResults& g);

    void unserialise(const std::string& s, ContactPointSpec& g);
    void unserialise(const std::string& s, ContactPointParams& g);
    void unserialise(const std::string& s, ContactPointResults& g);
  }
}
