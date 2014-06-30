//!
//! Contains the implementation of the squared exponetial kernel.
//!
//! \file world/kernel.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "kernel.hpp"
#include <iostream>

namespace obsidian
{
  namespace world
  {
    Eigen::MatrixXd sqExp(const Eigen::MatrixXd &x1, const Eigen::MatrixXd &x2,
        const Eigen::VectorXd &lengthScale, bool noisy)
    {
      // assert(x1.rows() == x2.rows())
      int n1 = x1.cols(), n2 = x2.cols();

      // Compute the weighted square distances
      Eigen::VectorXd w = (lengthScale.array().square().cwiseInverse()).matrix();
      Eigen::MatrixXd xx1 = w.replicate(1, n1).cwiseProduct(x1).cwiseProduct(x1).colwise().sum();
      Eigen::MatrixXd xx2 = w.replicate(1, n2).cwiseProduct(x2).cwiseProduct(x2).colwise().sum();
      Eigen::MatrixXd x1x2 = w.replicate(1, n1).cwiseProduct(x1).transpose() * x2;
      
      // Compute the covariance matrix
      Eigen::MatrixXd K = (-0.5 *
        Eigen::MatrixXd::Zero(n1, n2).cwiseMax(
          xx1.transpose().replicate(1, n2) + xx2.replicate(n1, 1) - 2 * x1x2)).array().exp();

      if (noisy) {
        K += K.colwise().sum().asDiagonal();
      }

      return K;
    }

    Eigen::MatrixXd sqExp2d(const Eigen::Matrix<double, 2, -1> &x1,
        const Eigen::Matrix<double, 2, -1> &x2,
        const Eigen::Vector2d &lengthScale, bool noisy)
    {
      // assert(x1.rows() == x2.rows())
      int n1 = x1.cols(), n2 = x2.cols();

      // Compute the weighted square distances
      Eigen::Vector2d w = (lengthScale.array().square().cwiseInverse()).matrix();
      Eigen::RowVectorXd xx1 = w.replicate(1, n1).cwiseProduct(x1).cwiseProduct(x1).colwise().sum();
      Eigen::RowVectorXd xx2 = w.replicate(1, n2).cwiseProduct(x2).cwiseProduct(x2).colwise().sum();
      Eigen::MatrixXd x1x2 = w.replicate(1, n1).cwiseProduct(x1).transpose() * x2;
      
      // Compute the covariance matrix
      Eigen::MatrixXd K = (-0.5 *
        Eigen::MatrixXd::Zero(n1, n2).cwiseMax(
          xx1.transpose().replicate(1, n2) + xx2.replicate(n1, 1) - 2 * x1x2)).array().exp();

      if (noisy) {
        K += K.colwise().sum().asDiagonal();
      }

      return K;
    }

    Eigen::Vector2d autoLengthScale(const std::pair<double,double> &x1x2, const std::pair<double,double> &y1y2,
        uint resx, uint resy)
    {
      double xLS = 0.5 * (x1x2.second - x1x2.first) / ((double)resx - 0.99999);
      double yLS = 0.5 * (y1y2.second - y1y2.first) / ((double)resy - 0.99999);
      return Eigen::Vector2d(xLS, yLS);
    }
  }
}
