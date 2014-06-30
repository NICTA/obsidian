//!
//! \file serial/commstypes.cpp
//! \author Lachlan McCalman
//! \date February, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "comms/datatypes.hpp"
#include <iostream>

namespace stateline
{
  namespace comms
  {

    std::string serialise(uint id)
    {
      return std::string((char*) &id, sizeof(uint) / sizeof(char));
    }

    void unserialise(const std::string& s, uint& id)
    {
      id = *((uint*) &s[0]);
    }

    void unserialise(const std::string& s, std::vector<uint>& ids)
    {
      std::stringstream lineStream(s);
      uint num;
      while (lineStream >> num)
        ids.push_back(num);
    }

    std::string serialise(const std::vector<uint>& ids)
    {
      std::stringstream result;
      std::copy(ids.begin(), ids.end(), std::ostream_iterator<uint>(result, " "));
      std::string s = result.str();
      s.erase(s.end() - 1);
      return s;
    }

  // std::string serialise(const GeoJobTypes& j)
  // {
  // return serialise((uint)j);
  // }

  // void unserialise(const std::string& s, GeoJobTypes& j)
  // {
  // uint uj;
  // unserialise(s,uj);
  // j = (GeoJobTypes)uj;
  // }

  }// namespace comms
} // namespace obsidian
