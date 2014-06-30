//!
//! Contains the interface for the gravity forward model.
//!
//! \file fwdmodel/gravity.hpp
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
    //! Compute the gravity sensitivity matrix.
    //!
    //! \param xEdges The x coordinates of the mesh grid.
    //! \param yEdges The y coordinates of the mesh grid.
    //! \param zEdges The z coordinates of the mesh grid.
    //! \param locations A Nx3 matrix containing the coordinates of the sensor locations.
    //!
    Eigen::MatrixXd gravSens(const Eigen::VectorXd &xEdges, const Eigen::VectorXd &yEdges, const Eigen::VectorXd &zEdges,
                             const Eigen::MatrixXd &locations);

  } // namespace fwd
} // namespace obsidian
