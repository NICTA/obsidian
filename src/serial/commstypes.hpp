//!
//! Serialisation of comms/datatypes.hpp elements
//!
//! \file serial/commstypes.hpp
//! \author Lachlan McCalman
//! \date February, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <string>
#include <vector>
#include "datatype/datatypes.hpp"

namespace stateline
{
  namespace comms
  {
    std::string serialise(uint id);
    void unserialise(const std::string& s, uint& id);

    std::string serialise(const std::vector<uint>& ids);
    void unserialise(const std::string& s, std::vector<uint>& ids);
  }
}
