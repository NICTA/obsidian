//!
//! Input constructs for problem specification
//!
//! \file input.hpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

// Standard Library
// Prerequisites
#include <boost/program_options.hpp>
#include <glog/logging.h>
// Project
#include "datatype/datatypes.hpp"
#include "prior/prior.hpp"
#include "datatype/sensors.hpp"
#include "app/settings.hpp"
#include "serial/serial.hpp"
#include "fwdmodel/fwd.hpp"

namespace po = boost::program_options;

namespace obsidian
{
  //! Read input.obsidian file
  //!
  void readInputFile(const std::string& inputFilename, po::variables_map& vm);

  //! Initialise options for the world model
  //!
  void initWorldOptions(po::options_description & options);

  //! Initialise options for mcmc
  //!
  void initMCMCOptions(po::options_description & options);

  //! Initialise options for a specific forward model to read from input.obsidian or write to output.gdf
  //!
  template<ForwardModel f>
  void initSensorInputFileOptions(po::options_description & add_options);

  //! Parse mcmc settings from loaded input.obsidian file
  //!
  stateline::MCMCSettings parseMCMCSettings(const po::variables_map& vm);

  //! Parse which sensors are enabled from loaded input.obsidian file
  //!
  std::set<ForwardModel> parseSensorsEnabled(const po::variables_map& vm);

  //! Determine which section in input.obsidian we should read settings of a ForwardModel from
  //!
  template<ForwardModel f>
  std::string configHeading();

  //! Parse a Prior Object from input.obsidian
  //!
  template<typename SensorPrior>
  SensorPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled);

  //! Parse a spec object from input.obsidian
  //!
  template<typename SensorSpec>
  SensorSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled);

  //! Parse a params object from input.obsidian
  //!
  template<typename SensorParams>
  SensorParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> &sensorsEnabled);

  //! Parse initial state of for each stack from input.obsidian.
  //!
  template<typename SensorParams>
  std::vector<SensorParams> parseInitStates(const po::variables_map& vm, const WorldSpec & spec,
                                            const std::set<ForwardModel> & sensorsEnabled);

  //! Parse a Result Object from input.obsidian
  //!
  template<typename SensorResult>
  SensorResult parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled);

  //! Write a object into output.gdf
  //!
  template<typename Object>
  po::variables_map write(const std::string & prefix, Object spec, const po::options_description & vm);

  //! Determine if input.obsidian wants us to run our simulation for generating results objects for read them form input.obsidian
  //!
  bool isSimulation(const po::variables_map& vm);

  //! Determine which properties are used by which forward model.
  //!
  template<ForwardModel f>
  void enableProperties(Eigen::VectorXi & propMasksMask);

  //! Method called for validating world spec and parameters.
  //!
  bool validateWorld(const WorldSpec & world, const WorldParams & param);

  //! Method called for validating world spec and parameters.
  //!
  template<typename Spec, typename Result>
  bool validateSensor(const WorldSpec & world, const Spec & spec, const Result & result);

}
