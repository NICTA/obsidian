//!
//! Contains the basic GDF datatypes for communicating with the MT forward model.
//!
//! \file datatype/mt.hpp
//! \author Lachlan McCalman
//! \author Darren Shen
//! \date April, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/forwardmodels.hpp"

namespace obsidian
{
  enum class MtAnisoLikelihood
  {
    GAUSSIAN, INVERSEGAMMA, Count
  };

  /**
   * Initial Parameters for MT forward Model.
   */
  struct MtAnisoSpec
  {
    /**
     * Nx3 Matrix of observation locations. Each row represents a point (x y z)
     */
    Eigen::MatrixXd locations;

    /**
     * Vector of MT frequencies
     */
    std::vector<Eigen::VectorXd> freqs;

    /**
     * Noise model used for MT
     */
    NoiseSpec noise;

    /**
     * A boole that lets you switch between aniso and iso
     */
    bool ignoreAniso;

  };

  struct MtAnisoCache
  {
    std::vector<world::InterpolatorSpec> boundaryInterpolation;
    world::Query query;
  };

  /**
   * Update parameters for initialised MT forward Model.
   */
  struct MtAnisoParams
  {
    bool returnSensorData;
  };

  /**
   * Contains the results of a forward aniso MT simulation.
   */
  struct MtAnisoResults
  {
    double likelihood;

    /**
     * Matrix of (z-axis) MT readings. Each element is a observation point.
     */
    std::vector<Eigen::MatrixX4cd> readings;
    std::vector<Eigen::MatrixX4d> phaseTensor;
    std::vector<Eigen::VectorXd> alpha;
    std::vector<Eigen::VectorXd> beta;
  };

  template<>
  struct Types<ForwardModel::MTANISO>
  {
    using Spec = MtAnisoSpec;
    using Cache = MtAnisoCache;
    using Params = MtAnisoParams;
    using GlobalParams = WorldParams;
    using Results = MtAnisoResults;
  };

  template<>
  inline std::string forwardModelLabel<ForwardModel::MTANISO>()
  {
    return "MT";
  }

} // namespace obsidian
