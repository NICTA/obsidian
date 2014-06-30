// Copyright (c) 2014, NICTA. 
// This file is licensed under the General Public License version 3 or later.
// See the COPYRIGHT file.

/**
 * Prior object for world params
 *
 * @file world.hpp
 * @author Lachlan McCalman
 * @date 2014
 */

#pragma once
#include <random>
#include <Eigen/Core>
#include "datatype/world.hpp"
#include "datatype/datatypes.hpp"
#include "distrib/multigaussian.hpp"
#include <glog/logging.h>

namespace obsidian
{
  namespace prior
  {

    class WorldParamsPrior
    {
    public:
      // construct
      WorldParamsPrior(const std::vector<distrib::MultiGaussian>& ctrlpts_, const std::vector<Eigen::MatrixXi>& ctrlptMasks_,
                       const std::vector<Eigen::MatrixXd>& ctrlptMins_, const std::vector<Eigen::MatrixXd>& ctrlptMaxs_,
                       const Eigen::VectorXd& ctrlptCoupledSds_, const Eigen::VectorXd& ctrlptUncoupledSds_,
                       const std::vector<distrib::MultiGaussian>& properties_, const std::vector<Eigen::VectorXi>& propMasks_,
                       const std::vector<Eigen::VectorXd>& propMins_, const std::vector<Eigen::VectorXd>& propMaxs_,
                       const std::vector<BoundaryClass>& classes_)
          : ctrlptMasks(ctrlptMasks_),
            ctrlptMins(ctrlptMins_),
            ctrlptMaxs(ctrlptMaxs_),
            ctrlptCoupledSds(ctrlptCoupledSds_),
            ctrlptUncoupledSds(ctrlptUncoupledSds_),
            ctrlptPrior(ctrlpts_),
            propMasks(propMasks_),
            propMins(propMins_),
            propMaxs(propMaxs_),
            propertyPrior(properties_),
            classes(classes_)
      {
        for (uint l = 0; l < propMasks.size(); l++)
        {
          for (uint p = 0; p < propMasks_[l].size(); p++)
          {
            if (!propMasks_[l](p))
            {
              propertyPrior[l].sigma.row(p).setZero();
              propertyPrior[l].sigma.col(p).setZero();
              propertyPrior[l].sigma(p, p) = 1;
            }
          }
        }
        for (uint l = 0; l < ctrlptMasks_.size(); l++)
        {
          Eigen::MatrixXi masks = ctrlptMasks_[l];
          masks.resize(masks.rows() * masks.cols(), 1);
          for (uint p = 0; p < masks.rows(); p++)
          {
            if (!masks(p))
            {
              ctrlptPrior[l].sigma.row(p).setZero();
              ctrlptPrior[l].sigma.col(p).setZero();
              ctrlptPrior[l].sigma(p, p) = 1;
            }
          }
        }
        WorldParams minParams; 
        WorldParams maxParams;
        minParams.rockProperties = propMins;
        maxParams.rockProperties = propMaxs;
        minParams.controlPoints = ctrlptMins;
        maxParams.controlPoints = ctrlptMaxs;
        thetaMin = deconstruct(minParams);
        thetaMax = deconstruct(maxParams);
        VLOG(1) << "Theta min:" << thetaMin.transpose();
        VLOG(1) << "Theta max:" << thetaMax.transpose();
      }
      
      Eigen::VectorXd thetaMinBound();

      Eigen::VectorXd thetaMaxBound();

      // build a WorldParams object from a flat vector
      WorldParams reconstruct(const Eigen::VectorXd& theta);

      // Evaluate log likelihood of theta under this prior
      double evaluatePDF(const Eigen::VectorXd& theta);

      // randomly sample a valid set of parameters
      Eigen::VectorXd sample(std::mt19937 &gen);

      // Method to compute the size of the prior
      uint size();

      // turn a WorldParams object into a flat vector
      Eigen::VectorXd deconstruct(const WorldParams& params);

      // private:
      std::vector<Eigen::MatrixXi> ctrlptMasks;
      std::vector<Eigen::MatrixXd> ctrlptMins;
      std::vector<Eigen::MatrixXd> ctrlptMaxs;
      Eigen::VectorXd ctrlptCoupledSds;
      Eigen::VectorXd ctrlptUncoupledSds;
      std::vector<distrib::MultiGaussian> ctrlptPrior;
      std::vector<Eigen::VectorXi> propMasks;
      std::vector<Eigen::VectorXd> propMins;
      std::vector<Eigen::VectorXd> propMaxs;
      std::vector<distrib::MultiGaussian> propertyPrior;
      std::vector<BoundaryClass> classes;
      Eigen::VectorXd thetaMin;
      Eigen::VectorXd thetaMax;
    };
  } //namespace prior
} //namespace obsidian
