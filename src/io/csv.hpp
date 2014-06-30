//!
//! Contains the interface of various IO interop with CSV
//!
//! \file io/csv.hpp
//! \author Nahid Akbar
//! \date 2014-06-06
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Core>
#include <string>
#include <vector>
#include "string.hpp"

namespace obsidian
{
  namespace io
  {
    //! For reading and writing csv files
    namespace csv
    {

      //! Read csv file into a list of list of string fields
      //!
      std::vector<std::vector<std::string>> readRaw(const std::string& inCsvFilePath);

      //! Write list of list of string fields into a csv file
      //!
      void writeRaw(const std::string& outCsvFilePath, const std::vector<std::vector<std::string>> & data);

      //! Extracts row and column dimensions of data in list of list of string fields format.
      //! \note Has checks for rectangularity of the data
      //! \note Assumes there is at least one row.
      //!
      std::pair<size_t, size_t> getRowMatrixDimensions(const std::vector<std::vector<std::string>> & data);

      //! Read csv file into a rectangular matrix
      //! \note Assumes underlying file is rectangular
      //!
      template<typename ScalarType, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
      Eigen::Matrix<ScalarType, Rows, Cols> read(const std::string& inCsvFilePath)
      {
        std::vector<std::vector<std::string>> raw = readRaw(inCsvFilePath);
        std::pair<size_t, size_t> dims = getRowMatrixDimensions(raw);

        Eigen::Matrix<ScalarType, Rows, Cols> mat;
        if (Cols == Eigen::Dynamic)
        {
          mat.resize(dims.first, dims.second);
        } else
        {
          dims.second = Cols;
          mat.resize(dims.first, Cols);
        }
        if (dims.first > 0 && dims.second > 0)
        {
          for (uint i = 0; i < dims.first; i++)
          {
            for (uint j = 0; j < dims.second; j++)
            {

              mat(i, j) = from_string<ScalarType>(raw[i][j]);
            }
          }
        }
        return mat;
      }

      //! Write Matrix into csv file
      //!
      template<typename ScalarType, int Rows = Eigen::Dynamic, int Cols = Eigen::Dynamic>
      void write(const std::string& outCsvFilePath, const Eigen::Matrix<ScalarType, Rows, Cols> &data)
      {
        std::vector<std::vector<std::string>> outdata(data.rows());
        for (int i = 0; i < data.rows(); i++)
        {
          outdata[i].resize(data.cols());
          for (int j = 0; j < data.cols(); j++)
          {
            outdata[i][j] = to_string(data(i, j));
          }
        }
        writeRaw(outCsvFilePath, outdata);
      }
    }
  }
}
