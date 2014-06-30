//!
//! Contains the interface for the magnetic forward model.
//!
//! \file fwdmodel/magnetic.hpp
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "fwdmodel/fwd.hpp"
#include "fwdmodel/gravmag.hpp"

namespace obsidian
{
  namespace fwd
  {
    //! Compute the magnetic sensitivity matrix.
    //! 
    //! \param xEdges The x coordinates of the mesh grid.
    //! \param yEdges The y coordinates of the mesh grid.
    //! \param zEdges The z coordinates of the mesh grid.
    //! \param locations A Nx3 matrix containing the coordinates of the
    //!                  sensor locations.
    //! \param bX, bY, bZ The field values along each axis. Assuming the
    //!                   field values are constant in each axis.
    //!
    Eigen::MatrixXd magSens(const Eigen::VectorXd &xEdges, const Eigen::VectorXd &yEdges, const Eigen::VectorXd &zEdges,
                            const Eigen::MatrixXd &locations, const double &bX, const double &bY, const double &bZ);

    //! Compute the magnetic sensitivity matrix.
    //! 
    //! \param xEdges The x coordinates of the mesh grid.
    //! \param yEdges The y coordinates of the mesh grid.
    //! \param zEdges The z coordinates of the mesh grid.
    //! \param locations A Nx3 matrix containing the coordinates of the
    //!                  sensor locations.
    //! \param bX, bY, bZ The field values at each of the mesh grid points.
    //!
    Eigen::MatrixXd magSens(const Eigen::VectorXd &xEdges, const Eigen::VectorXd &yEdges, const Eigen::VectorXd &zEdges,
                            const Eigen::MatrixXd &locations, const Eigen::VectorXd &bX, const Eigen::VectorXd &bY,
                            const Eigen::VectorXd &bZ);

  } // namespace fwd
} // namespace obsidian
