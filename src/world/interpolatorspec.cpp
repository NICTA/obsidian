//!
//! Contains the implementation for interpolation.
//!
//! \file world/interpolatorspec.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "world/interpolate.hpp"
#include "world/interpolatorspec.hpp"
#include "world/grid.hpp"
#include "world/kernel.hpp"

namespace obsidian
{
  namespace world
  {
    InterpolatorSpec::InterpolatorSpec(const std::pair<double, double>& xBounds, const std::pair<double, double>& yBounds,
                                       const std::pair<double, double>& zBounds, const BoundarySpec& boundaryspec)
        : offsetXBounds(xBounds), offsetYBounds(yBounds)
    {
      // We need to set up an automatic grid here...
      resolution = boundaryspec.ctrlPointResolution;
      floorHeight = zBounds.second;

      controlPointX = edgeGrid2D(xBounds, yBounds, resolution.first, resolution.second); // make same as matlab
      offsetFunction = boundaryspec.offset;
      lengthScale_ = autoLengthScale(xBounds, yBounds, resolution.first, resolution.second);
      Eigen::MatrixXd kernel = sqExp2d(controlPointX.transpose(), controlPointX.transpose(), lengthScale_, true);

      // Al: Modify kernel (See Matlab...)
      kernel.diagonal() += kernel.colwise().sum(); // takes care of KXX + diag(sum(KXX))
      L_ = kernel; // prepare for some solving!
      boundaryClass = boundaryspec.boundaryClass;
    }

    InterpolatorSpec::InterpolatorSpec()
        : resolution(0, 0),
          controlPointX(0, 2),
          boundaryClass(BoundaryClass::Normal),
          offsetFunction(1, 1),
          offsetXBounds(std::make_pair(0, 1)),
          offsetYBounds(std::make_pair(0, 1)),
          lengthScale_(1, 2),
          L_()
    {
      floorHeight = 0.0;
    }

    Eigen::MatrixXd InterpolatorSpec::getWeights(const Eigen::MatrixXd &query) const
    {
      // (also see how L_ is computed)
      Eigen::MatrixXd weights = L_.solve(sqExp2d(query.transpose(), controlPointX.transpose(), lengthScale_, false).transpose());

      // Normalise the weights
      uint nQuery = weights.cols();
      uint nCtrl = weights.rows();
      for (uint i = 0; i < nQuery; i++)
      {
        double total = 0.0;
        for (uint j = 0; j < nCtrl; j++)
          total += weights(j, i); // should be all positive
        for (uint j = 0; j < nCtrl; j++)
          weights(j, i) /= total;
      }
      return weights;
    }

    int InterpolatorSpec::numControlPoints() const
    {
      return controlPointX.rows();
    }
  }
}
