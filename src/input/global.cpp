//!
//! Input implementations related to global objects.
//!
//! \file global.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"

namespace obsidian
{

  template<>
  GlobalSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel>& sensorsEnabled)
  {
    LOG(INFO)<< "Parsing problem specification";
    GlobalSpec spec;
    return
    { parseSpec<WorldSpec>(vm, sensorsEnabled),
      parseSpec<GravSpec>(vm, sensorsEnabled),
      parseSpec<MagSpec>(vm, sensorsEnabled),
      parseSpec<MtAnisoSpec>(vm, sensorsEnabled),
      parseSpec<Seismic1dSpec>(vm, sensorsEnabled),
      parseSpec<ContactPointSpec>(vm, sensorsEnabled),
      parseSpec<ThermalSpec>(vm, sensorsEnabled)};
  }
  template<>
  GlobalParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel>& sensorsEnabled)
  {
    return
    { parseSimulationParams<WorldParams>(vm, sensorsEnabled),
      parseSimulationParams<GravParams>(vm, sensorsEnabled),
      parseSimulationParams<MagParams>(vm, sensorsEnabled),
      parseSimulationParams<MtAnisoParams>(vm, sensorsEnabled),
      parseSimulationParams<Seismic1dParams>(vm, sensorsEnabled),
      parseSimulationParams<ContactPointParams>(vm, sensorsEnabled),
      parseSimulationParams<ThermalParams>(vm, sensorsEnabled)
    };
  }
  template<>
  GlobalResults parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel>& sensorsEnabled)
  {
    return
    { parseSensorReadings<GravResults>(vm, sensorsEnabled),
      parseSensorReadings<MagResults>(vm, sensorsEnabled),
      parseSensorReadings<MtAnisoResults>(vm, sensorsEnabled),
      parseSensorReadings<Seismic1dResults>(vm, sensorsEnabled),
      parseSensorReadings<ContactPointResults>(vm, sensorsEnabled),
      parseSensorReadings<ThermalResults>(vm, sensorsEnabled)
    };
  }

  template<>
  GlobalPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return
    {
      parsePrior<prior::WorldParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::GravParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::MagParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::MtAnisoParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::Seismic1dParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::ContactPointParamsPrior>(vm, sensorsEnabled),
      parsePrior<prior::ThermalParamsPrior>(vm, sensorsEnabled)
    };

  }
}
