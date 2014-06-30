//!
//! Contains the interface for computing layer transitions.
//!
//! \file world/transitions.hpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/world.hpp"
#include "world/query.hpp"
#include "world/interpolate.hpp"

namespace obsidian
{
  namespace world
  {
    //! Get the depths of each layer at each query point.
    //! 
    //! \param boundaries The interpolator specs for each layer.
    //! \param inputs The world model parameters.
    //! \param query The query containing the query points.
    //! 
    //! \return A matrix with rows containing the depths of the query points at
    //!         every layer.
    //!
    Eigen::MatrixXd getTransitions(const std::vector<world::InterpolatorSpec>& boundaries,
        const WorldParams &inputs, const Query &query);

    //! Get the thickness of each layer at each query point.
    //! 
    //! \param transitions A nlayers by mqueries matrix of the transitions between layers at each sensor location.
    //! \return An mqueries x nthicknesses matrix of layer thicknesses at each sensor location.
    //!
    Eigen::MatrixXd thickness(const Eigen::MatrixXd& transitions);

    //! Code for post-processing the granites, applying a non-linear transform
    //! to their heights.
    //!
    Eigen::MatrixXd postProcessGranites(const Eigen::MatrixXd& transitioni,
        Eigen::VectorXd offseti, const Eigen::MatrixXd& transitionj,
        const Query& query, const uint region, const Eigen::MatrixXd& inputs,
        double floorHeight);

  }
}
