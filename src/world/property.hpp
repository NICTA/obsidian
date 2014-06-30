//!
//! Contains the interface for the world properties.
//!
//! \file world/property.hpp
//! \author Lachlan McCalman
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/datatypes.hpp"

namespace obsidian
{
  namespace world
  {
    //! Extract a particular property from the WorldParams.
    //! 
    //! \param input the worldParams object.
    //! \param desiredProp the desired property.
    //! \return A vector of that property for each layer in the world.
    //!
    Eigen::VectorXd extractProperty(const WorldParams& input, obsidian::RockProperty desiredProp);

  } // namespace world
} // namespace obsidian

