//!
//! Contains the typedef for the different forward models.
//!
//! \file forwardmodels.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "base.hpp"
#include "datatype/voxel.hpp"
#include "datatype/noise.hpp"
#include "world/interpolate.hpp"

namespace obsidian
{
  enum class ForwardModel
  {
    PRIOR, GRAVITY, MAGNETICS, MTANISO, SEISMIC1D, CONTACTPOINT, THERMAL, Count
  };

  template<ForwardModel f>
  struct Types;

  template<ForwardModel>
  std::string forwardModelLabel();
}
