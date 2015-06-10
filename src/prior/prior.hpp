// Copyright (c) 2014, NICTA. 
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * The prior module produces distributions over model parameters.
 * Other parts of the program can sample and evaluate probabilities
 * that are mapped into an associated real vector space. In other words,
 * we 'vectorize' objects to a 'theta' vector of doubles, but only the parts
 * that are changing.
 *
 * @file prior.hpp
 * @author Lachlan McCalman
 * @date 2014
 */

#pragma once

// Standard Library
#include <string>
// Prerequisites
#include <glog/logging.h>
#include <Eigen/Core>
// Project
#include "datatype/datatypes.hpp"
#include "prior/world.hpp"
#include "prior/gravity.hpp"
#include "prior/magnetic.hpp"
#include "prior/mt.hpp"
#include "prior/seismic.hpp"
#include "prior/contactpoint.hpp"
#include "prior/thermal.hpp"
#include <random>

namespace obsidian
{
  class GlobalPrior
  {
  public:
//      GlobalPrior(const std::vector<distrib::MultiGaussian>& ctrlpts,
//                  const std::vector<Eigen::MatrixXi>& ctrlptMasks,
//                  const std::vector<distrib::MultiGaussian>& properties,
//                  const std::vector<Eigen::VectorXi>& propMasks);

    GlobalParams reconstruct(const Eigen::VectorXd& theta) const;
    double evaluate(const Eigen::VectorXd& theta);
    Eigen::VectorXd sample(std::mt19937 &gen);

    uint size();

//    private:
    prior::WorldParamsPrior world;
    prior::GravParamsPrior grav;
    prior::MagParamsPrior mag;
    prior::MtAnisoParamsPrior mt;
    prior::Seismic1dParamsPrior s1d;
    prior::ContactPointParamsPrior cpt;
    prior::ThermalParamsPrior thr;
  };

} // namespace obsidian

