//!
//! Contains the implementation of the world properties.
//!
//! \file world/property.cpp
//! \author Lachlan McCalman
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "world/property.hpp"
#include <Eigen/Dense>
#include <glog/logging.h>

namespace obsidian
{
  namespace world
  {
    //! Bound the density property.
    //!
    //! \param raw The raw density value.
    //! \return Bounded density value.
    //!
    double boundDensity(double raw)
    {
      // Density must be strictly greater than zero
      double density = std::max(raw, 0.0);
      return density;
    }

    //! Bound the log susceptibility.
    //!
    //! \param raw The raw log susceptibility value.
    //! \return Bounded log susceptibility value.
    //!
    double boundLogSusceptibility(double raw)
    {
      // No bound
      double logSus = raw;
      return logSus;
    }

    //! Bound the thermal conductivity.
    //!
    //! \param raw The raw thermal conductivity value.
    //! \return Bounded thermal conductivity value.
    //!
    double boundThermalConductivity(double raw)
    {
      // Must be greater than some small epsilon
      double minConductivity = 1e-3;
      double conductivity = std::max(raw, minConductivity);
      return conductivity;
    }

    //! Bound the thermal productivity.
    //!
    //! \param raw The raw thermal productivity value.
    //! \return Bounded thermal productivity value.
    //!
    double boundThermalProductivity(double raw)
    {
      // Density must be strictly greater than zero
      double productivity = std::max(raw, 0.0);
      return productivity;
    }

    //! Bound the log resistivity.
    //!
    //! \param raw The raw log resistivity value.
    //! \return Bounded log resistivity value.
    //!
    double boundLogResistivity(double raw)
    {
      double resistivity = raw;
      return resistivity;
    }
    
    //! Bound the resistivity.
    //!
    //! \param raw The raw resistivity value.
    //! \return Bounded resistivity value.
    //!
    double boundResistivity(double raw)
    {
      // Must be strictly greater than zero
      double resistivity = std::max(raw, 0.0);
      return resistivity;
    }

    //! Bound the resistivity phase.
    //!
    //! \param raw The raw resistivity phase value.
    //! \return Bounded resistivity phase value.
    //!
    double boundResistivityPhase(double raw)
    {
      double minPhase = 0.0;
      double maxPhase = M_PI * 0.5;

      // Must be strictly greater than zero
      double phase = std::min(std::max(raw, minPhase), maxPhase);
      return phase;
    }

    //! Bound the primary wave velocity.
    //!
    //! \param raw The raw primary wave velocity value.
    //! \return Bounded primary wave velocity value.
    //!
    double boundPWaveVelocity(double raw)
    {
      // Must be strictly greater than zero
      double resistivity = std::max(raw, 0.0);
      return resistivity;
    }

    //! Bound the susceptibility.
    //!
    //! \param raw The raw susceptibility value.
    //! \return Bounded susceptibility value.
    //!
    double boundSusceptibility(double raw)
    {
      // Must be strictly greater than zero
      double susceptibility = std::max(raw, 0.0);
      return susceptibility;
    }

    Eigen::VectorXd extractProperty(const WorldParams& input, obsidian::RockProperty desiredProp) 
    {
      const uint isSuss = desiredProp == RockProperty::Susceptibility;
      const uint isResX = desiredProp == RockProperty::ResistivityX;
      const uint isResY = desiredProp == RockProperty::ResistivityY;
      const uint isResZ = desiredProp == RockProperty::ResistivityZ;
      std::vector<Eigen::VectorXd> rocks = input.rockProperties;
      uint nLayers = rocks.size();
      uint propIndex =   (uint)(isSuss * (uint)RockProperty::LogSusceptibility) 
                       + (uint)(isResX * (uint)RockProperty::LogResistivityX)  
                       + (uint)(isResY * (uint)RockProperty::LogResistivityY)  
                       + (uint)(isResZ * (uint)RockProperty::LogResistivityZ)  
                       + (uint)((!isSuss) && (!isResX) && (!isResY) && (!isResZ)) * (uint)desiredProp;
      CHECK(propIndex < (uint)RockProperty::Count);
      Eigen::VectorXd properties(nLayers);

      for (uint i=0; i<nLayers; i++)
        properties[i] = rocks[i](propIndex);

      switch (desiredProp)
      {
        case RockProperty::Density:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundDensity(properties[i]);
          break;
        case RockProperty::LogSusceptibility:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundLogSusceptibility(properties[i]);
          break;
        case RockProperty::ThermalConductivity:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundThermalConductivity(properties[i]);
          break;
        case RockProperty::ThermalProductivity:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundThermalProductivity(properties[i]);
          break;
        case RockProperty::LogResistivityX:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundLogResistivity(properties[i]);
          break;
        case RockProperty::LogResistivityY:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundLogResistivity(properties[i]);
          break;
        case RockProperty::LogResistivityZ:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundLogResistivity(properties[i]);
          break;
        case RockProperty::ResistivityPhase:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundResistivityPhase(properties[i]);
          break;
        case RockProperty::PWaveVelocity:
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundPWaveVelocity(properties[i]);
          break;
        case RockProperty::Susceptibility:
          properties=(properties.array()*std::log(10)).exp();
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundSusceptibility(properties[i]);
          break;
        case RockProperty::ResistivityX:
          properties=(properties.array()*std::log(10)).exp();
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundResistivity(properties[i]);
          break;
        case RockProperty::ResistivityY:
          properties=(properties.array()*std::log(10)).exp();
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundResistivity(properties[i]);
          break;
        case RockProperty::ResistivityZ:
          properties=(properties.array()*std::log(10)).exp();
          for (uint i=0; i<nLayers; i++)
            properties[i] = boundResistivity(properties[i]);
          break;
        default:
          break;
      }
      return properties;
    }
  } // namespace world
} // namespace obsidian
