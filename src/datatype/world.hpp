//!
//! World specification types.
//!
//! \file datatype/world.hpp
//! \author Lachlan McCalman
//! \date December 2013
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "base.hpp"

namespace obsidian
{
  //!
  //!
  enum class RockProperty
  {
    Density,
    LogSusceptibility,
    ThermalConductivity,
    ThermalProductivity,
    LogResistivityX,
    LogResistivityY,
    LogResistivityZ,
    ResistivityPhase,
    PWaveVelocity,
    Count,
    Susceptibility, // Note: these are derived property and does not count towards the number of rock properties
    ResistivityX,
    ResistivityY,
    ResistivityZ
  };

  enum class BoundaryClass
  {
    Normal, // TODO: document
    Warped // TODO: document
  };

  //!
  struct BoundarySpec
  {
    //! the boundary z (down) offset
    Eigen::MatrixXd offset;
    //! control points x * y
    std::pair<uint, uint> ctrlPointResolution;
    //! control points x * y
    BoundaryClass boundaryClass;
  };

  //!
  class WorldSpec
  {
  public:

    WorldSpec()
    {
    }

    WorldSpec(double x1, double x2, double y1, double y2, double z1, double z2)
        : xBounds(x1, x2), yBounds(y1, y2), zBounds(z1, z2)
    {
    }

    //! min, max x (east)
    std::pair<double, double> xBounds;
    //! min, max y (north)
    std::pair<double, double> yBounds;
    //! min, max z (down)
    std::pair<double, double> zBounds;
    //!
    std::vector<BoundarySpec> boundaries;
    //! if true interpret boundaries[1:] as times and use PWaveVelocity to calculate later offsets
    bool boundariesAreTimes;
  };

  //! These are optimised
  struct WorldParams
  {
    std::vector<Eigen::VectorXd> rockProperties;
    std::vector<Eigen::MatrixXd> controlPoints;
  };

} // namespace obsidian
