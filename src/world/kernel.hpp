//!
//! Contains the interface for Gaussian Process kernels.
//!
//! \file world/kernel.hpp
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/world.hpp"

namespace obsidian
{
  namespace world
  {
    //! Compute a covariance matrix using the square exponential function.
    //! 
    //! \param x1, x2 DxN matrices. Each row represents the input in one dimension.
    //! \param lengthScale Vector containing length scale value for each dimension.
    //! \param noisy Whether noise is modelled.
    //!
    Eigen::MatrixXd sqExp(const Eigen::MatrixXd &x1, const Eigen::MatrixXd &x2,
        const Eigen::VectorXd &lengthScale, bool noisy = false);

    //! Compute a covariance matrix using the squared exponential function.
    //! This function is faster than sqExp, but only applies when the dimensions
    //! of the points is 2 (which is true for control points and query points).
    //! 
    //! \param x1, x2 2xN matrices. Each row represents the input in one dimension.
    //! \param lengthScale Vector containing length scale value for each dimension.
    //! \param noisy Whether noise is modelled.
    //!
    Eigen::MatrixXd sqExp2d(const Eigen::Matrix<double, 2, -1> &x1,
        const Eigen::Matrix<double, 2, -1> &x2,
        const Eigen::Vector2d &lengthScale, bool noisy = false);

    //! Get an appropriate length scale given the grid resolution and a smoothing factor.
    //! 
    //! \param x1x2 The start and end of the grid boundaries in the x direction.
    //! \param y1y2 The start and end of the grid boundaries in the y direction.
    //! \param resx The resolution of the grid in the x direction.
    //! \param resy The resolution of the grid in the y direction.
    //! \param smoothing The smoothing factor.
    //! 
    //! \returns A two dimensional vector containing the length scales for the
    //!          x and y dimensions.
    //!
    Eigen::Vector2d autoLengthScale(const std::pair<double,double> &x1x2,
        const std::pair<double,double> &y1y2, uint resx, uint resy);
  }
}
