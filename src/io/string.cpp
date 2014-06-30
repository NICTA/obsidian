//!
//! \file io/string.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014-04-26
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <string>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "io/string.hpp"

namespace obsidian
{
  namespace io
  {
    std::vector<std::string> &split(const std::string &s, const char delim, std::vector<std::string> &elems)
    {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim))
      {
        boost::algorithm::trim(item);
        if (item.size() > 0)
          elems.push_back(item);
      }
      return elems;
    }

    std::vector<std::string> split(const std::string &s, const char delim)
    {
      std::vector<std::string> elems;
      split(s, delim, elems);
      return elems;
    }

    std::string join(const std::vector<std::string> &s, const std::string delim)
    {
      std::ostringstream ss;
      for (uint i = 0; i < s.size(); i++)
      {
        ss << s[i];
        if (i != s.size() - 1)
          ss << delim;
      }
      return ss.str();
    }
  }
}
