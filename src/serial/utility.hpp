//!
//! Contains the basic GDF datatypes for communication between forward models.
//!
//! \file serial/utility.hpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
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
    const int D = Eigen::Dynamic;
    // helper functions
    template<typename T, int R, int C>
    std::string matrixString(const Eigen::Matrix<T, R, C>& m)
    {
      if (m.rows() > 0 && m.cols() > 0)
      {
        return std::string((char *) m.data(), m.size() * sizeof(T));
      } else
      {
        return "";
      }
    }
    template<typename T = double, int R = D, int C = D>
    Eigen::Matrix<T, R, C> stringMatrix(const std::string &s, uint rows)
    {
      if (rows > 0 and s.length() >= rows * sizeof(T))
      {
        Eigen::Matrix<T, R, C> m(rows, C == D ? s.length() / sizeof(T) / rows : C);
        // we're just going to copy the data from the string
        // directly into the matrix
        memcpy(m.data(), s.c_str(), s.length());
        return m;
      } else
      {
        return Eigen::Matrix<T, R, C>();
      }
    }
    std::string cmplxMatrixString(const Eigen::MatrixXcd& m);
    std::string vectorString(const Eigen::VectorXd& m);
    Eigen::VectorXd stringVector(const std::string &s);
    Eigen::MatrixXcd stringCmplxMatrix(const std::string &s, uint rows);

    /**
     * Serialises an initialised Protobuf object into a string
     */
    template<class ProtobufType>
    inline std::string protobufToString(const ProtobufType &pb)
    {
      std::string result;
      pb.SerializeToString(&result);
      return result;
    }
  }
}
