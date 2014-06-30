//!
//! Contains the interface of various IO interop with NumPy.
//!
//! \file io/npy.hpp
//! \author Lachlan McCalman
//! \date January 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Dense>

#include "cnpy/cnpy.h"

namespace obsidian
{
  namespace io
  {
    //! Read from an NPY file containing a single floating point matrix.
    //! 
    //! \param filename The name of the file.
    //! \return The matrix from the file.
    //!
    Eigen::MatrixXd readNPY(const std::string& filename);

    //! Read from an NPY file containing a single integer matrix.
    //! 
    //! \param filename The name of the file.
    //! \return The matrix from the file.
    //!
    Eigen::MatrixXi readNPYi(const std::string& filename);

    //! Read from an NPY file containing a single complex matrix.
    //! 
    //! \param filename The name of the file.
    //! \return The matrix from the file.
    //!
    Eigen::MatrixXcd readNPYc(const std::string& filename);

    //! Read from a specific key of an NPZ file containing a single
    //! floating point matrix.
    //! 
    //! \param filename The name of the file.
    //! \param key The key for the matrix.
    //! \return The matrix from the file.
    //!
    Eigen::MatrixXd readNPZ(const std::string& filename, const std::string& key);

    // NOTE DEPRECATED: use the NpzWriter class instead
    // /**
    //  * Write to a NPY file. This type of a file stores a single matrix.
    //  *
    //  * @param filename The name of the file
    //  * @param matrix The matrix to store
    //  */
    void writeNPY(const std::string& filename, const Eigen::MatrixXd& matrix);
    // 
    // /**
    //  * Write to a NPZ file. This type of files can have multiple matrices stored
    //  * as one file. The matrices are indexed by name.
    //  *
    //  * @param filename The name of the NPZ file
    //  * @param mats A map containing keys and their corresponding matrices
    //  * @param overwrite Whether to overwrite any existing file with same filename
    //  */
    void writeNPZ(const std::string &filename, const std::map<std::string, Eigen::MatrixXd> &mats, bool overwrite = false);

    //! Used for writing to an NPZ file.
    //!
    class NpzWriter
    {
      public:
        //! Create a new writer for an NPZ file.
        //!
        //! \param filename The name of the file.
        //!
        NpzWriter(const std::string &filename);

        template<typename T>
        void write(const std::string &name,
            const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> rowMat)
        {
          const unsigned int shape[] =
          {
            static_cast<unsigned int>(rowMat.rows()),
            static_cast<unsigned int>(rowMat.cols())
          };

          cnpy::npz_save(filename_, name, // name of the file and matrix
              rowMat.data(), // the matrix data
              shape, // the matrix shape
              2, // the number of dimensions
              written_ ? "a" : "w"); // overwrite file on first iteration

          written_ = true;
        }

        template<typename T>
        void writeScalar(const std::string &name, const T value)
        {
          Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> mat(1,
              1);
          mat(0, 0) = value;
          write(name, mat);
        }

    private:
      std::string filename_;
      bool written_ = false;
    };
  }
}
