//!
//! Contains the voxelization data types.
//!
//! \file datatype/voxel.hpp
//! \author Lachlan McCalman
//! \date December 2013
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

namespace obsidian
{
  struct VoxelSpec
  {
    uint xResolution;
    uint yResolution;
    uint zResolution;
    uint supersample;
  };
} // namespace obsidian
