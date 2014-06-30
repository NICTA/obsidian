//!
//! Contains the basic GDF datatypes for 1D seismic simulations.
//!
//! \file datatype/seismic.hpp
//! \author Lachlan McCalman
//! \date May, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/forwardmodels.hpp"

namespace obsidian
{
  /**
   * Initial Parameters for Seismic1D forward model. These parameters are required
   * only at the beginning of a set of simulations, to define the observation
   * locations and the prism boundaries the discretise the world
   */
  struct Seismic1dSpec
  {
    // shape: nLocations x 2 (x, y)
    Eigen::MatrixXd locations;
    // nLocations x what layer are they intersecting
    std::vector<Eigen::VectorXi> interfaces;
    // Noise
    NoiseSpec noise;
  };

  /**
   * Update Parameters for initialised seismic forward model.
   */
  struct Seismic1dParams
  {
    bool returnSensorData;
  };

  struct Seismic1dCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
  };

  /**
   * Contains the results of a forward Seismic 1d simulation.
   */
  struct Seismic1dResults
  {
    double likelihood;
    /**
     * Vector containing a set of reflection times for each sensor location (may be
     * empty)
     */
    std::vector<Eigen::VectorXd> readings;
  };

  template<>
  struct Types<ForwardModel::SEISMIC1D>
  {
    using Spec = Seismic1dSpec;
    using Cache = Seismic1dCache;
    using Params = Seismic1dParams;
    using GlobalParams = WorldParams;
    using Results = Seismic1dResults;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::SEISMIC1D>()
  {
    return "Seismic 1D";
  }

} // namespace obsidian

