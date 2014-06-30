//!
//! Contains the interface for creating grids.
//!
//! \file world/grid.hpp
//! \author Alistair Reid
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

    // A decent grid function
    Eigen::MatrixXd internalGrid2D(std::pair<double,double> xMinMax, std::pair<double,double> yMinMax, uint resx, uint resy);

    Eigen::MatrixXd internalGrid2DX(std::pair<double,double> xMinMax, std::pair<double,double> yMinMax, uint resx, uint resy);

    // another decent grid function
    Eigen::MatrixXd edgeGrid2D(std::pair<double,double> xMinMax, std::pair<double,double> yMinMax, uint resx, uint resy);

    //! Maps all the columns of a matrix into a single column vector. Equivalent to the
    //! colon operator A(:) in MATLAB.
    //! 
    //! \param matrix The matrix to flatten.
    //! \return A column vector containing the flattened matrix.
    //!
    Eigen::VectorXd flatten(const Eigen::MatrixXd &matrix);

    //! Maps all the columns of a matrix into a single column vector. Equivalent to the
    //! colon operator A(:) in MATLAB.
    //! 
    //! \param matrix The matrix to flatten.
    //! \return A column vector containing the flattened matrix.
    //!
    Eigen::VectorXi flatten(const Eigen::MatrixXi& matrix);
    
    //! Compute a regular 2D grid given the boundaries and resolutions in
    //! the x and y directions.
    //! 
    //! \param boundaryStart, boundaryEnd The start and end boundaries.
    //! \param resolution The resolution in each direction (npoints in each direction).
    //! \param padding Number of cells to extend outside the boundaries.
    //! \return 
    //!
    std::pair<Eigen::MatrixXd, std::pair<Eigen::MatrixXd, Eigen::MatrixXd>>
    makeGrid(const Eigen::Vector2d &boundaryStart, const Eigen::Vector2d &boundaryEnd,
      const Eigen::Vector2i &resolution, int padding = 0);

    Eigen::MatrixXd sensorGrid(const WorldSpec& worldSpec, uint resx, uint resy, double sensorZ);
    Eigen::MatrixXd sensorGrid3d(const WorldSpec& worldSpec, uint resx, uint resy, uint resz);

  }
}
