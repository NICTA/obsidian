//!
//! String helper functions
//!
//! \file io/string.hpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014-04-26
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace obsidian
{
  namespace io
  {
    //! Split string
    //! \return Split String; ignores empty tokens
    std::vector<std::string> split(const std::string &stringToSplit, const char delimiterForSplitting = ' ');

    //! Concatenate String
    //! \return concatenated string
    std::string join(const std::vector<std::string> &stringArray, const std::string glue = " ");

    //! Parse value from string
    template<typename ValueType> ValueType from_string(const std::string & stringToParse)
    {
      ValueType v;
      std::istringstream iss(stringToParse);
      iss >> v;
      return v;
    }

    //! Convert Value to String
    template<typename ValueType> std::string to_string(const ValueType & v)
    {
      std::ostringstream oss;
      oss << v;
      return oss.str();
    }

    //! Convert Value to String
    template<typename ValueType, typename ... Vr> std::string to_string(const ValueType & v, Vr ... vr)
    {
      return to_string(v) + " " + to_string(vr...);
    }

    //! Convert bool to string
    template<> inline std::string to_string<bool>(const bool & v)
    {
      std::ostringstream oss;
      oss << (v ? "true" : "false");
      return oss.str();
    }

    //! Convert float to string with 10 significant digits
    template<> inline std::string to_string<float>(const float & v)
    {
      std::ostringstream oss;
      oss << std::setprecision(10) << v;
      return oss.str();
    }

    //! Convert double to string with 20 significant digits
    template<> inline std::string to_string<double>(const double & v)
    {
      std::ostringstream oss;
      oss << std::setprecision(20) << v;
      return oss.str();
    }

    //! Convert long double to string with 40 significant digits
    template<> inline std::string to_string<long double>(const long double & v)
    {
      std::ostringstream oss;
      oss << std::setprecision(40) << v;
      return oss.str();
    }
  }
}
