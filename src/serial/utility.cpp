//!
//! \file serial/utility.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "utility.hpp"

namespace obsidian
{
  namespace comms
  {
    std::string cmplxMatrixString(const Eigen::MatrixXcd& m)
    {
      // Make a string from the bytes
      return std::string((char *) m.data(), m.size() * sizeof(m(0, 0)));
    }

    std::string vectorString(const Eigen::VectorXd& m)
    {
      // Make a string from the bytes
      return std::string((char *) m.data(), m.size() * sizeof(m(0)));
    }

    Eigen::VectorXd stringVector(const std::string &s)
    {
      Eigen::VectorXd m(s.length() / sizeof(double));

      // we're just going to copy the data from the string
      // directly into the matrix
      memcpy(m.data(), s.c_str(), s.length());
      return m;
    }

    Eigen::MatrixXcd stringCmplxMatrix(const std::string &s, uint rows)
    {
      Eigen::MatrixXcd m(rows, s.length() / sizeof(std::complex<double>) / rows);

      // we're just going to copy the data from the string
      // directly into the matrix
      memcpy(m.data(), s.c_str(), s.length());
      return m;
    }
  }
}
