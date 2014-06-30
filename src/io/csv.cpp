//!
//! Contains the interface of various IO interop with CSV
//!
//! \file io/csv.cpp
//! \author Nahid Akbar
//! \date 2014-06-06
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "csv.hpp"
#include <iostream>
#include <fstream>
#include <glog/logging.h>

namespace obsidian
{
  namespace io
  {
    namespace csv
    {
      std::vector<std::vector<std::string>> readRaw(const std::string& filename)
      {
        std::vector<std::vector<std::string>> data;
        std::ifstream ifs(filename);
        if (!ifs.good())
        {
          LOG(ERROR)<< "Could not read " << filename;
          exit(EXIT_FAILURE);
        }
        for (std::string row; getline(ifs, row);)
        {
          // delete all comments from the row
          auto comment = row.find('#');
          if (comment != std::string::npos)
            row.erase(comment);
          
          auto rowdata = split(row, ',');

          if (!rowdata.empty())
            data.push_back(rowdata);
        }
        return data;
      }
      void writeRaw(const std::string& filename, const std::vector<std::vector<std::string>> & data)
      {
        std::ofstream ofs(filename);
        for (const std::vector<std::string> & row : data)
        {
          ofs << join(row, ", ") << std::endl;
        }
      }
      std::pair<size_t, size_t> getRowMatrixDimensions(const std::vector<std::vector<std::string>> & data)
      {
        if (data.size() > 0 && data[0].size() > 0)
        {
          size_t rows = data.size();
          size_t cols = data[0].size();
          for (size_t row = 0; row < rows; row++)
          {
            if (data[row].size() < cols)
            {
              LOG(ERROR)<< "Input File ROW " << row << " has " << data[row].size() << " COLS < " << cols << " expected COLS";
            }
          }
          return std::make_pair(rows, cols);
        }
        return std::make_pair(0, 0);
      }
    }
  }
}
