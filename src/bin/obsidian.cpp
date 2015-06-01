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
  GlobalSpec globalSpec = parseSpec<GlobalSpec>(vm, sensorsEnabled);
  std::vector<world::InterpolatorSpec> interp = world::worldspec2Interp(globalSpec.world);
  GlobalPrior prior = parsePrior<GlobalPrior>(vm, sensorsEnabled);
  GlobalResults results = loadResults(globalSpec.world, vm, sensorsEnabled);
    
  //cache for gravity in this case
  GravCache gravCache = fwd::generateCache<ForwardModel::GRAVITY>(interp, globalSpec.world, globalSpec.grav);

  // Image we're sampling according to some smart stategy in this bound
  // (stateline's entire job is choosing points to evaluate within this bound)
  Eigen::VectorXd minTheta = prior.world.thetaMinBound();
  Eigen::VectorXd maxTheta = prior.world.thetaMaxBound();

  // The above structures are expensive to construct,
  // but actually running the sensor model (below) is cheaper, and is done many
  // times
  for (uint i=0; i<1000;i++)
  {
    // Imagine this theta has been given to us by stateline (instead of sampled
    // from prior)
    Eigen::VectorXd theta = prior.sample(gen);


    // reconstruct the associated world, then evaluate its likelihood
    GlobalParams globalParams = prior.reconstruct(theta);
    GravResults synthetic = fwd::forwardModel<ForwardModel::GRAVITY>(globalSpec.grav, gravCache, globalParams.world);
    synthetic.likelihood = lh::likelihood<ForwardModel::GRAVITY>(synthetic, results.grav, globalSpec.grav);
    std::cout << synthetic.likelihood << std::endl;
  }

  return 0;
}
