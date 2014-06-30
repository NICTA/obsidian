// Copyright (c) 2014, NICTA. 
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.
#include "prior/prior.hpp"
#include "prior/world.hpp"

namespace obsidian
{

//  GlobalPrior::GlobalPrior(const std::vector<distrib::MultiGaussian>& ctrlpts,
//        const std::vector<Eigen::MatrixXi>& ctrlptMasks,
//        const std::vector<distrib::MultiGaussian>& properties,
//        const std::vector<Eigen::VectorXi>& propMasks)
//  :world(ctrlpts, ctrlptMasks, properties, propMasks)
//  {
//  }

  GlobalParams GlobalPrior::reconstruct(const Eigen::VectorXd& theta)
  {
    GlobalParams g;
    uint start = 0;
    uint len = world.size();
    g.world = world.reconstruct(theta.segment(start, len));
    return g;
  }

  double GlobalPrior::evaluate(const Eigen::VectorXd& theta)
  {
    double logPDF = 0;
    uint start = 0;
    uint len = world.size();
    logPDF += world.evaluatePDF(theta.segment(start, len));
    start += len;
    // elaborate for other priors...
    return logPDF;
  }

  Eigen::VectorXd GlobalPrior::sample(std::mt19937 &gen)
  {
    Eigen::VectorXd worldTheta = world.sample(gen);
    // Concatenate
    Eigen::VectorXd totalTheta = worldTheta;
    return totalTheta;
  }

  uint GlobalPrior::size()
  {
    uint totalSize = 0;
    totalSize += world.size();
    return totalSize;
  }

} // namespace obsidian
