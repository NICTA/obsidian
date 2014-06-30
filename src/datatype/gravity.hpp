//!
//! Contains gravity forward model datatypes.
//!
//! \file datatype/gravity.hpp
//! \author Lachlan McCalman
//! \date December 2013
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/forwardmodels.hpp"

namespace obsidian
{
  /**
   * Initial Parameters for Gravity forward Model. These parameters are required
   * only at the beginning of a set of simulations, to define the observation
   * locations and the prism boundaries the discretise the world
   */
  struct GravSpec
  {
    // shape: nLocations x 3
    Eigen::MatrixXd locations;

    // The voxelisation parameters
    VoxelSpec voxelisation;

    NoiseSpec noise;
  };

  struct GravCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
    Eigen::MatrixXd sensitivityMatrix;
    Eigen::MatrixXi sensorIndices;
    Eigen::MatrixXd sensorWeights;
  };

  /**
   * Update Parameters for initialised gravity forward Model.
   */
  struct GravParams
  {
    bool returnSensorData;
  };

  /**
   * Contains the results of a forward gravity simulation.
   */
  struct GravResults
  {
    double likelihood;

    /**
     * Vector of (z-axis) gravity readings -- may be empty
     */
    Eigen::VectorXd readings;
  };

  template<>
  struct Types<ForwardModel::GRAVITY>
  {
    using Spec = GravSpec;
    using Cache = GravCache;
    using Params = GravParams;
    using GlobalParams = WorldParams;
    using Results = GravResults;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::GRAVITY>()
  {
    return "Gravity";
  }

} // namespace obsidian

