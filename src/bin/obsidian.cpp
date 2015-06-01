//!
//! A delegator that runs parallel tempering on several workers to perform an inversion.
//!
//! \file obsidian.cpp
//! \author Lachlan McCalman
//! \author Darren Shen
//! \author Nahid Akbar
//! \date February, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

// Standard Library
#include <random>
#include <thread>
#include <chrono>
#include <numeric>
// Prerequisites
#include <glog/logging.h>
#include <boost/program_options.hpp>
// Project
#include "app/console.hpp"
#include "app/settings.hpp"
#include "input/input.hpp"
#include "comms/delegator.hpp"
#include "comms/requester.hpp"
#include "serial/serial.hpp"
#include "app/asyncdelegator.hpp"
#include "app/signal.hpp"
#include "infer/mcmc.hpp"
#include "infer/metropolis.hpp"
#include "infer/adaptive.hpp"
#include "fwdmodel/fwd.hpp"
#include "datatype/sensors.hpp"
#include "detail.hpp"
#include "likelihood/likelihood.hpp"

using namespace obsidian;
using namespace stateline;

namespace ph = std::placeholders;

// Command line options specific to obsidian
po::options_description commandLineOptions()
{
  po::options_description cmdLine("Delegator Command Line Options");
  cmdLine.add_options() //
  ("port,p", po::value<uint>()->default_value(5555), "TCP port to accept connections") //
  ("configfile,c", po::value<std::string>()->default_value("obsidian_config"), "configuration file") //
  ("inputfile,i", po::value<std::string>()->default_value("input.obsidian"), "input file") //
  ("recover,r", po::bool_switch()->default_value(false), "force recovery")
  ("anneallength,a", po::value<uint>()->default_value(1000), "anneal chains with n samples before starting mcmc");
  return cmdLine;
}
    
int main(int ac, char* av[])
{
  std::random_device rd;
  std::mt19937 gen(rd());

  // Get the settings from the command line
  auto vm = init::initProgramOptions(ac, av, commandLineOptions());

  readConfigFile(vm["configfile"].as<std::string>(), vm);
  readInputFile(vm["inputfile"].as<std::string>(), vm);

  std::set<ForwardModel> sensorsEnabled = parseSensorsEnabled(vm);
  DelegatorSettings delegatorSettings = parseDelegatorSettings(vm);
  GlobalSpec globalSpec = parseSpec<GlobalSpec>(vm, sensorsEnabled);
  WorldSpec& worldSpec = globalSpec.world;
  std::vector<world::InterpolatorSpec> interp = world::worldspec2Interp(worldSpec);
  GlobalPrior prior = parsePrior<GlobalPrior>(vm, sensorsEnabled);
  GlobalResults results = loadResults(worldSpec, vm, sensorsEnabled);
  
  Eigen::VectorXd theta = prior.sample(gen);
  GlobalParams globalParams = prior.reconstruct(theta);
  WorldParams worldParams = globalParams.world;
    
  if (sensorsEnabled.count(ForwardModel::GRAVITY))
  {
    GravResults real = results.grav;
    GravSpec& gravSpec = globalSpec.grav;
    GravParams& params = globalParams.grav;

    GravCache cache = fwd::generateCache<ForwardModel::GRAVITY>(interp, worldSpec, gravSpec);
    GravResults synthetic = fwd::forwardModel<ForwardModel::GRAVITY>(gravSpec, cache, worldParams);
    synthetic.likelihood = lh::likelihood<ForwardModel::GRAVITY>(synthetic, real, gravSpec);
  }

  // if (enabled.count(ForwardModel::MAGNETICS))
  // {
  //   MagCache mag;
  // }
  // if (enabled.count(ForwardModel::MTANISO))
  // {
  //   MtAnisoCache mt;
  // }
  // if (enabled.count(ForwardModel::SEISMIC1D))
  // {
  //   Seismic1dCache s1d;
  // }
  // if (enabled.count(ForwardModel::CONTACTPOINT))
  // {
  //   ContactPointCache cpoint;
  // }
  // if (enabled.count(ForwardModel::THERMAL))
  // {
  //   ThermalCache therm;
  // }

  // auto proposal = std::bind(&mcmc::adaptiveGaussianProposal,ph::_1, ph::_2,
  //                           prior.world.thetaMinBound(), prior.world.thetaMaxBound()); 

  return 0;
}
