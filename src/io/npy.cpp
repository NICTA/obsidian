//!
//! \file io/npy.cpp
//! \author Lachlan McCalman
//! \date January 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "npy.hpp"

namespace obsidian
{
  namespace io
  {
    void writeNPY(const Eigen::MatrixXd& matrix, const std::string& filename)
    {
      uint rows = matrix.rows();
      uint cols = matrix.cols();
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMat(rows, cols);
      rowMat = matrix;
      const uint shape[] = { rows, cols };
      const double* data = rowMat.data();
      cnpy::npy_save(filename, data, shape, 2, "w");
    }

    Eigen::MatrixXd readNPY(const std::string& filename)
    {
      cnpy::NpyArray arr = cnpy::npy_load(filename);
      double* loaded_data = reinterpret_cast<double*>(arr.data);
      assert(arr.word_size == sizeof(double));
      assert(arr.shape.size() == 2);
      uint rows = arr.shape[0];
      uint cols = arr.shape[1];
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(rows, cols);
      uint counter = 0;
      for (uint i = 0; i < rows; i++)
      {
        for (uint j = 0; j < cols; j++)
        {
          mat(i, j) = loaded_data[counter];
          counter++;
        }
      }
      return mat;
    }

    Eigen::MatrixXi readNPYi(const std::string& filename)
    {
      cnpy::NpyArray arr = cnpy::npy_load(filename);
      int32_t* loaded_data = reinterpret_cast<int32_t*>(arr.data);
      assert(arr.word_size == sizeof(int32_t));
      assert(arr.shape.size() == 2);
      uint rows = arr.shape[0];
      uint cols = arr.shape[1];
      Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(rows, cols);
      uint counter = 0;
      for (uint i = 0; i < rows; i++)
      {
        for (uint j = 0; j < cols; j++)
        {
          mat(i, j) = loaded_data[counter];
          counter++;
        }
      }
      return mat;
    }

    Eigen::MatrixXcd readNPYc(const std::string& filename)
    {
      cnpy::NpyArray arr = cnpy::npy_load(filename);
      double* loaded_data = reinterpret_cast<double*>(arr.data);
      assert(arr.word_size == sizeof(double));
      assert(arr.shape.size() == 2);
      uint rows = arr.shape[0];
      uint cols = arr.shape[1];
      Eigen::MatrixXcd mat(rows, cols / 2);
      uint counter = 0;
      for (uint i = 0; i < rows; i++)
      {
        for (uint j = 0; j < cols / 2; j++)
        {
          double re = loaded_data[counter++];
          double im = loaded_data[counter++];
          mat(i, j) = std::complex<double>(re, im);
        }
      }
      return mat;
    }

    Eigen::MatrixXd readNPZ(const std::string& filename, const std::string& key)
    {
      cnpy::NpyArray arr = cnpy::npz_load(filename, key);
      double* loaded_data = reinterpret_cast<double*>(arr.data);
      assert(arr.word_size == sizeof(double));
      assert(arr.shape.size() == 2);
      uint rows = arr.shape[0];
      uint cols = arr.shape[1];
      Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> mat(rows, cols);
      uint counter = 0;
      for (uint i=0;i<rows;i++)
      {
        for (uint j=0;j<cols;j++)
        {
          mat(i,j) = loaded_data[counter];
          counter++;
        }
      }
      return mat;
    }

    void writeNPZ(const std::string &filename, const std::map<std::string, Eigen::MatrixXd> &mats, bool overwrite)
    {
      // Loop through each key/value pair in mats
      // kv->first is the name of the matrix
      // kv->second is the matrix in col-major order
      for (auto kv = mats.cbegin(); kv != mats.cend(); ++kv)
      {
        // Convert to row-major order
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMat = kv->second;

        const unsigned int shape[] = { static_cast<unsigned int>(rowMat.rows()), static_cast<unsigned int>(rowMat.cols()) };

        cnpy::npz_save(filename, kv->first, // name of the file and matrix
                       rowMat.data(), // the matrix data
                       shape, // the matrix shape
                       2, // the number of dimensions
                       kv == mats.cbegin() && overwrite ? "w" : "a"); // Overwrite file on first iteration
      }
    }

    NpzWriter::NpzWriter(const std::string& filename)
        : filename_(filename)
    {
    }

  }
}
