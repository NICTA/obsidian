//!
//! Common header files included by all the input implementations. Contains helper functions.
//!
//! \file common.hpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "input.hpp"
#include "io/npy.hpp"
#include "io/string.hpp"
#include "io/csv.hpp"

#include <Eigen/Core>

namespace Eigen
{
  //! for reading from boost::program_options
  //!
  inline void validate(boost::any& v, const std::vector<std::string>& values, Eigen::Vector2d*, int)
  {
    std::vector<std::string> input = obsidian::io::split(values[0]);
    Eigen::Vector2d output;
    if (input.size() == 2)
    {
      output(0) = std::stod(input[0]);
      output(1) = std::stod(input[1]);
    } else
    {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = output;
  }

  //! for reading from boost::program_options
  //!
  inline void validate(boost::any& v, const std::vector<std::string>& values, Eigen::Vector3i*, int)
  {
    std::vector<std::string> input = obsidian::io::split(values[0]);
    Eigen::Vector3i output;
    if (input.size() == 3)
    {
      output(0) = std::stoi(input[0]);
      output(1) = std::stoi(input[1]);
      output(2) = std::stoi(input[2]);
    } else
    {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = output;
  }

  //! for reading from boost::program_options
  //!
  inline void validate(boost::any& v, const std::vector<std::string>& values, Eigen::Vector3d*, int)
  {
    std::vector<std::string> input = obsidian::io::split(values[0]);
    Eigen::Vector3d output;
    if (input.size() == 3)
    {
      output(0) = std::stod(input[0]);
      output(1) = std::stod(input[1]);
      output(2) = std::stod(input[2]);
    } else
    {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = output;
  }

  //! for reading from boost::program_options
  //!
  inline void validate(boost::any& v, const std::vector<std::string>& values, Eigen::VectorXd*, int)
  {
    std::vector<std::string> input = obsidian::io::split(values[0]);
    Eigen::VectorXd output(input.size());
    try
    {
      for (uint i = 0; i < input.size(); i++)
      {
        output(i) = std::stod(input[i]);
      }
    } catch (...)
    {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = output;
  }

}

namespace obsidian
{
  //! for reading a rectangular portion of list of list of fields as a Matrix
  //!
  template<typename T, int C, int Offset>
  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> readFixed(const std::vector<std::vector<std::string>> & data)
  {
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> results(data.size(), C);
    for (uint i = 0; i < data.size(); i++)
    {
      for (uint j = 0; j < C; j++)
      {
        results(i, j) = io::from_string<T>(data[i][Offset + j]);
      }
    }
    return results;
  }

  //! for writing a matrix into a list of list of fields
  //!
  template<typename T>
  void writeFixed(std::vector<std::vector<std::string>> & data, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> & obj)
  {
    if (data.size() < static_cast<size_t>(obj.rows()))
    {
      data.resize(obj.rows());
    }
    for (uint i = 0; i < obj.rows(); i++)
    {
      for (uint j = 0; j < obj.cols(); j++)
      {
        data[i].push_back(io::to_string<T>(obj(i, j)));
      }
    }
  }

  //! for reading a ragged array (vector of matrix) from a list of list of fields
  //!
  template<typename T, int Offset>
  std::vector<Eigen::Matrix<T, Eigen::Dynamic, 1>> readRagged(std::vector<std::vector<std::string>> & data)
  {
    std::vector<Eigen::Matrix<T, Eigen::Dynamic, 1>> results;
    for (uint i = 0; i < data.size(); i++)
    {
      Eigen::Matrix<T, Eigen::Dynamic, 1> freqs(data[i].size() - Offset);

      for (uint j = Offset; j < data[i].size(); j++)
      {
        freqs[j - Offset] = io::from_string<T>(data[i][j]);
      }
      results.push_back(freqs);
    }
    return results;
  }

  //! for wtiting a ragged array (vector of matrix) into a list of list of fields
  //!
  template<typename T>
  void writeRagged(std::vector<std::vector<std::string>> & data, std::vector<Eigen::Matrix<T, Eigen::Dynamic, 1>> & obj)
  {
    if (data.size() < obj.size())
    {
      data.resize(obj.size());
    }
    for (uint i = 0; i < obj.size(); i++)
    {
      for (uint j = 0; j < obj[i].rows(); j++)
      {
        data[i].push_back(io::to_string<T>(obj[i](j)));
      }
    }
  }

  //! helper method for building boost::program_options::variable_map
  //!
  inline po::variables_map build_vm(po::variables_map vm, const po::options_description & od, const std::string & topic,
                                    const std::map<std::string, std::string> & map)
  {
    std::ostringstream oss;
    oss << "[" << topic << "]" << std::endl;
    for (auto kv : map)
    {
      oss << kv.first << " = " << kv.second << std::endl;
    }
    std::istringstream iss(oss.str());
    po::store(po::parse_config_file(iss, od), vm);
    return vm;
  }

}

