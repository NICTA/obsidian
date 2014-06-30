//!
//! Contains the basic GDF datatypes for communication between forward models.
//!
//! \file datatype/thermal.hpp
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
   * Initial Parameters for Thermal forward Model. These parameters are required
   * only at the beginning of a set of simulations, to define the observation
   * locations and the prism boundaries the discretise the world
   */
  struct ThermalSpec
  {
    // shape: nLocations x 3
    Eigen::MatrixXd locations;

    // top boundary condition
    double surfaceTemperature;

    // bottom boundary condition
    double lowerBoundary;

    // interpert lower boundary condition as heat flow rather than temperature
    bool lowerBoundaryIsHeatFlow;

    // The voxelisation parameters
    VoxelSpec voxelisation;

    NoiseSpec noise;
  };

  struct ThermalCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
    std::pair<double, double> xBounds;
    std::pair<double, double> yBounds;
    std::pair<double, double> zBounds;
  };

  /**
   * Update Parameters for initialised thermal forward Model.
   */
  struct ThermalParams
  {
    bool returnSensorData;
  };

  /**
   * Contains the results of a forward thermal simulation.
   */
  struct ThermalResults
  {
    double likelihood;

    /**
     * Vector of (z-axis) gravity readings -- may be empty
     */
    Eigen::VectorXd readings;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::THERMAL>()
  {
    return "Thermal";
  }

  template<>
  struct Types<ForwardModel::THERMAL>
  {
    using Spec = ThermalSpec;
    using Cache = ThermalCache;
    using Params = ThermalParams;
    using GlobalParams = WorldParams;
    using Results = ThermalResults;
  };

} // namespace obsidian

