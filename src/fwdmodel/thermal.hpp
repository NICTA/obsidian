//!
//! Contains the interface for the thermal forward model.
//!
//! \file fwdmodel/thermal.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "fwdmodel/fwd.hpp"
#include <boost/multi_array.hpp>

namespace obsidian
{
  namespace fwd
  {
    boost::multi_array<double, 3> fillAndPad(const Eigen::VectorXd& values, const ThermalSpec& spec);

    boost::multi_array<double, 3> eastWest(const boost::multi_array<double, 3>& pad);

    boost::multi_array<double, 3> cells(const boost::multi_array<double, 3>& pad, uint axis);

    boost::multi_array<double, 3> isocells(const boost::multi_array<double, 3>& pad);

    boost::multi_array<double, 3> temp(const boost::multi_array<double, 3>& eastwest, const boost::multi_array<double, 3>& northsouth,
                                       const boost::multi_array<double, 3>& updown, const boost::multi_array<double, 3>& sCells,
                                       const Eigen::MatrixXd& tempZ0, const Eigen::MatrixXd& zLowBound, bool zLowBoundIsHeatFlow,
                                       double xSize, double ySize, double zSize);

  } // namespace fwd
} // namespace obsidian

