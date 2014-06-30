//!
//! Contains the GDF datatypes for the magnetic forward model
//!
//! \file datatype/magnetic.hpp
//! \author Lachlan McCalman
//! \date April, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/forwardmodels.hpp"

namespace obsidian
{
  /**
   * Initial Parameters for magnetism forward Model. These parameters are required
   * only at the beginning of a set of simulations, to define the observation
   * locations and the prism boundaries the discretise the world
   */
  struct MagSpec
  {
    // shape: nLocations x 3
    Eigen::MatrixXd locations;

    // The voxelisation parameters
    VoxelSpec voxelisation;

    // Noise
    NoiseSpec noise;

    //! The background magnetic field for TMA calculations
    Eigen::VectorXd backgroundField;

  };

  struct MagCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
    Eigen::MatrixXd sensitivityMatrix;
    Eigen::MatrixXi sensorIndices;
    Eigen::MatrixXd sensorWeights;
  };

  /**
   * Update Parameters for initialised magnetism forward Model.
   */
  struct MagParams
  {
    bool returnSensorData;
  };

  /**
   * Contains the results of a forward magnetism simulation.
   */
  struct MagResults
  {
    double likelihood;

    /**
     * Vector of (z-axis) magnetism readings -- may be empty
     */
    Eigen::VectorXd readings;

  };

  template<>
  struct Types<ForwardModel::MAGNETICS>
  {
    using Spec = MagSpec;
    using Cache = MagCache;
    using Params = MagParams;
    using GlobalParams = WorldParams;
    using Results = MagResults;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::MAGNETICS>()
  {
    return "Magnetic";
  }

} // namespace obsidian

