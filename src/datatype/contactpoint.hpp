//!
//! Contains the basic GDF datatypes for Contact point sensors.
//!
//! \file datatype/contactpoint.hpp
//! \author Lachlan McCalman
//! \date May, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!
/**
 *
 *
 * @file contactpoint.hpp
 * @author Lachlan McCalman
 * @date 2014
 */

#pragma once

#include "datatype/forwardmodels.hpp"

namespace obsidian
{
  /**
   * Initial Parameters for Seismic1D forward model. These parameters are required
   * only at the beginning of a set of simulations, to define the observation
   * locations and the prism boundaries the discretise the world
   */
  struct ContactPointSpec
  {
    // shape: nLocations x 2 (x, y)
    Eigen::MatrixXd locations;
    // nLocations x what layer are they intersecting
    std::vector<Eigen::VectorXi> interfaces;

    // Noise
    NoiseSpec noise;
  };

  struct ContactPointCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
  };

  /**
   * Update Parameters for initialised seismic forward model.
   */
  struct ContactPointParams
  {
    bool returnSensorData;
  };

  /**
   * Contains the results of a forward Seismic 1d simulation.
   */
  struct ContactPointResults
  {
    double likelihood;
    /**
     * Vector containing the intersection depth for each measurement
     */
    std::vector<Eigen::VectorXd> readings;
  };

  template<>
  struct Types<ForwardModel::CONTACTPOINT>
  {
    using Spec = ContactPointSpec;
    using Cache = ContactPointCache;
    using Params = ContactPointParams;
    using GlobalParams = WorldParams;
    using Results = ContactPointResults;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::CONTACTPOINT>()
  {
    return "Contact Point";
  }

} // namespace obsidian

