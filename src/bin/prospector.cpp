//!
//! Output samples from the prior in Obsidian format
//!
//! \file prospecter.cpp
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
#include "fwdmodel/global.hpp"
#include "datatype/sensors.hpp"
#include "io/npy.hpp"
#include "io/dumpnpz.hpp"
#include "likelihood/likelihood.hpp"
#include "detail.hpp"

using namespace obsidian;
using namespace stateline;

namespace ph = std::placeholders;

// Command line options specific to obsidian
po::options_description commandLineOptions()
{
  po::options_description cmdLine("Delegator Command Line Options");
  cmdLine.add_options() //
    ("configfile,c", po::value<std::string>()->default_value("obsidian_config"), "configuration file") //
    ("inputfile,i", po::value<std::string>()->default_value("input.obsidian"), "input file") //
    ("outputfile,o", po::value<std::string>()->default_value("output.npz"), "output file") //
    ("nsamples,n", po::value<uint>()->default_value(100), "The number of samples to output")//
    ("fwdmodel,f", po::bool_switch()->default_value(false), "Also compute forward models");//
  return cmdLine;
}

int main(int ac, char* av[])
{
  init::initialiseSignalHandler();
  // Get the settings from the command line
  auto vm = init::initProgramOptions(ac, av, commandLineOptions());

  // Set up a random generator here
  std::random_device rd;
  std::mt19937 gen(rd());

  readConfigFile(vm["configfile"].as<std::string>(), vm);
  readInputFile(vm["inputfile"].as<std::string>(), vm);
  std::set<ForwardModel> sensorsEnabled = parseSensorsEnabled(vm);
  GlobalPrior prior = parsePrior<GlobalPrior>(vm, sensorsEnabled);
  
  // Stuff needed for forward modelling
  GlobalSpec globalSpec = parseSpec<GlobalSpec>(vm, sensorsEnabled);
  std::vector<world::InterpolatorSpec> boundaryInterp = world::worldspec2Interp(globalSpec.world);
  GlobalResults realResults = loadResults(globalSpec.world, vm, sensorsEnabled);
  GlobalCache cache = fwd::generateGlobalCache(boundaryInterp, globalSpec, sensorsEnabled);

  // Concatenate all the chains and take samples from them
  uint nsamples = vm["nsamples"].as<uint>();

  std::vector<WorldParams> dumpParams;
  std::vector<double> dumpPrior;
  std::vector<Eigen::VectorXd> dumpTheta;

  // Forward modelling stuff
  bool computeForwardModels = vm["fwdmodel"].as<bool>();
  std::vector<GlobalResults> dumpResults;
  std::vector<std::vector<double>> dumpLikelihood;

  std::string filename = vm["outputfile"].as<std::string>();
  io::NpzWriter writer(filename);
  
  for (uint i = 0; i < nsamples; i++)
  {
    LOG(INFO) << "Generating sample " << i;
    if(global::interruptedBySignal)
    {
      break;
    }
    // get the state
    Eigen::VectorXd theta = prior.sample(gen);
    GlobalParams params = prior.reconstruct(theta);
    // prior
    double priorLikelihood = prior.evaluate(theta);
    dumpPrior.push_back(priorLikelihood);
    // add the state for writing
    dumpParams.push_back(params.world);
    dumpTheta.push_back(theta);
    if (computeForwardModels)
    {
      GlobalResults results = fwd::forwardModelAll(globalSpec, cache, params, sensorsEnabled);
      dumpResults.push_back(results);
      std::vector<double> likelihoods = lh::likelihoodAll(results, realResults, globalSpec, sensorsEnabled);
      dumpLikelihood.push_back(likelihoods); 
    }
  }
  dumpParamsNPZ(writer, dumpParams);
  dumpPriorNPZ(writer, dumpPrior);
  dumpThetaNPZ(writer, dumpTheta);
  if (computeForwardModels)
  {
    dumpResultsNPZ(writer, dumpResults);
    dumpLikelihoodNPZ(writer, dumpLikelihood);
  }

  return 0;
}
