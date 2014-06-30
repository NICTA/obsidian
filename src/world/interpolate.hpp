//!
//! Contains the interface for interpolation.
//!
//! \file world/interpolate.hpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Core>

#include "datatype/world.hpp"
#include "interpolatorspec.hpp"
#include "world/query.hpp"

namespace obsidian
{
  namespace world
  {
    //! Generate a vector of interpolators for each layer based on the
    //! key info contained in WorldSpec.
    //!
    //! \param region The world specifications.
    //!
    std::vector<InterpolatorSpec> worldspec2Interp(const WorldSpec& region);

    //! Given a 3D query, interpolate the depth of each point in the query.
    //! 
    //! \param query The 3D query.
    //! \param layer The layer to interpolate in.
    //! \param input The packed parameter values.
    //! 
    //! \returns A vector of depths that correspond to each query point.
    //!
    Eigen::VectorXd kernelInterpolate(const Query& query, const uint boundary, const Eigen::MatrixXd& input);

    //! Given a 3D query, interpolate the depth of each point in the query linearly.
    //! 
    //! \param query The 3D query.
    //! \param interpolator The interpolator specificaitons.
    //! 
    //! \returns A vector of depths that correspond to each query point.
    //!
    Eigen::VectorXd linearInterpolate(const Query& query, InterpolatorSpec interpolator);

  } // world namespace
} // gdf namespace
