//!
//! Contains the interface for the 1D MT forward model.
//!
//! \file fwdmodel/mt1d.hpp
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/datatypes.hpp"
#include "fwdmodel/fwd.hpp"
#include "world/interpolate.hpp"

namespace obsidian
{
  namespace fwd
  {
    //! Calculate the impedence matrix for anisotropic 1D MT.
    //! 
    //! \param thicknesses A vector containing the thicknesses for each layer.
    //! \param freqs A vector containing the MT frequencies.
    //! \param resx A vector containing the resistivities in x direction (not logarithmic).
    //! \param resy A vector containing the resistivities in y direction (not logarithmic).
    //! \param phases A vector containing the phase of each layer.
    //! \return The impedance matrix for anisotropic MT.
    //!
    Eigen::MatrixX4cd impedenceAniso1d(const Eigen::VectorXd &thicknesses, const Eigen::VectorXd &freqs, const Eigen::VectorXd &resx,
                                       const Eigen::VectorXd &resy, const Eigen::VectorXd &phases);

    namespace detail
    {
      //! Constant for the permeability of free space.
      //!
      constexpr double MU = 4 * M_PI * 1e-7;

      //! Rotate the impedance matrix based on the phases of the layers.
      //! 
      //! \param Z The impedance matrix.
      //! \param phaseBelow The phase of the layer below.
      //! \param phaseAbove The phase of the layer above.
      //! \return The rotated impedance matrix.
      //!
      Eigen::Matrix2cd applyRotation(const Eigen::Matrix2cd &Z, double phaseBelow, double phaseAbove);

      //! Calculates the primary angle of a vector.
      //! 
      //! \param y The imaginary part of the phase.
      //! \param x The real part of the phase.
      //! \return The primary angle of the vector.
      //!
      double primaryAngle(double y, double x);
    }

    //! Calculate the phase tensor given the impedance matrix.
    //! 
    //! \param Z The impedance matrix.
    //! \return The phase tensor.
    //!
    Eigen::MatrixX4d phaseTensor1d(const Eigen::MatrixX4cd &Z);

    //! Computes the alpha value of the phase tensor, which is a measure of its
    //! rotations from the major axis.
    //! 
    //! \param tensor The phase tensor computed by phaseTensor1d()
    //!
    Eigen::VectorXd alpha1d(const Eigen::MatrixX4d &tensor);

    //! Computes the beta value of the phase tensor, which is a measure of its
    //! rotations from the z-axis.
    //!
    //! \param tensor The phase tensor computed by phaseTensor1d()
    //!
    Eigen::VectorXd beta1d(const Eigen::MatrixX4d &tensor);
  }
}
