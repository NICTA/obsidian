//!
//! Contains the interface for grid voxelisation.
//!
//! \file voxelise.hpp
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Dense>
#include <boost/multi_array.hpp>
#include "datatype/datatypes.hpp"
#include "world/transitions.hpp"

namespace obsidian
{
  namespace world
  {
    //! Convert transitions into a grid of points.
    //! 
    //! \param transitions The layer transitions.
    //! \param zIntercepts A vector containing the z coordinates of the voxels.
    //! 
    //! \return A matrix of property values. Rows contain the property values
    //!         of each transition point at every depth specified by zIntercepts.
    //!         The property values are obtained from linear interpolation.
    //!
    Eigen::MatrixXd voxelise(const Eigen::MatrixXd &transitions,
        const Eigen::VectorXd &zIntercepts, const Eigen::VectorXd &props);
    
    //! Return a vector containing regularly spaced numbers within a range.
    //! This is equivalent to the colon operator in MATLAB (Note the different
    //! parameter ordering).
    //! 
    //! \param from The start of the range.
    //! \param to The end of the range (inclusive).
    //! \param step The step size.
    //!
    Eigen::VectorXd linrange(double from, double to, double step = 1.0);

    Eigen::MatrixXd getVoxels(const std::vector<world::InterpolatorSpec>& interpolators,
        const WorldParams& inputs, const Query& query, obsidian::RockProperty desiredProp);

    Eigen::VectorXd shrink3d(const Eigen::VectorXd &densities, int nx, int ny, int nz);

  }
}
