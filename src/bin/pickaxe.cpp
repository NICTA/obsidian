//!
//! Output the results of an obsidian run in open format.
//!
//! \file pickaxe.cpp
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
    ("nthin,t", po::value<uint>()->default_value(10), "The step size between samples (>0)") //
    ("burnin,b", po::value<uint>()->default_value(0), "The number of samples to discard from the beginning") //
    ("fwdmodel,f", po::bool_switch()->default_value(false), "Also compute forward models")//
    ("recover,r", po::bool_switch()->default_value(true), "recovery (deprecated)");
  return cmdLine;
}

//! Perform rejection sampling on a given set of samples.
//!
//! \param trueDist A function to evaluate the true distribution in log space.
//! \param propDist A function to evaluate the proposed distribution in log space.
//! \param samples Samples from the proposed distribution.
//! \return A list of samples drawn from the true distribution.
//!
template <class TruePdfFunc, class PropPdfFunc, class Sample>
std::vector<Sample> rejectionSample(const TruePdfFunc &trueDist,
    const PropPdfFunc &propDist, const std::vector<Sample> &samples)
{
  // Evaluate the true and proposed distribution for each of the samples in log space
  Eigen::VectorXd truePdf(samples.size());
  Eigen::VectorXd propPdf(samples.size());
  for (uint i = 0; i < samples.size(); i++)
  {
    truePdf(i) = -trueDist(samples[i]);
    propPdf(i) = -propDist(samples[i]);
  }

  // Scale the proposed distribution so that it is always larger than
  // the true distribution
  double logK = (truePdf - propPdf).maxCoeff();

  // Initialise the random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist;

  // List of samples sampled from the true distribution using samples
  // from the proposed distribution
  std::vector<Sample> draws;
  for (uint i = 0; i < samples.size(); i++)
  {
    double acceptRatio = truePdf(i) - (propPdf(i) + logK);
    if (std::log(dist(gen)) < acceptRatio)
    {
      draws.push_back(samples[i]);
    }
  }

  return draws;
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
  MCMCSettings mcmcSettings = parseMCMCSettings(vm);
  DBSettings dbSettings = parseDBSettings(vm);
  dbSettings.recover = true;
  GlobalPrior prior = parsePrior<GlobalPrior>(vm, sensorsEnabled);
  mcmc::Sampler mcmc(mcmcSettings, dbSettings, prior.size(), global::interruptedBySignal);
  mcmc::ChainArray chains = mcmc.chains();
  
  // Stuff needed for forward modelling
  GlobalSpec globalSpec = parseSpec<GlobalSpec>(vm, sensorsEnabled);
  std::vector<world::InterpolatorSpec> boundaryInterp = world::worldspec2Interp(globalSpec.world);
  std::vector<uint> sensorId;
  std::vector<std::string> sensorSpecData;
  std::vector<std::string> sensorReadings;

  GlobalResults realResults = loadResults(globalSpec.world, vm, sensorsEnabled);

  GlobalCache cache = fwd::generateGlobalCache(boundaryInterp, globalSpec, sensorsEnabled);

  // Concatenate all the chains and take samples from them
  uint nthin = vm["nthin"].as<uint>();
  uint burnin = vm["burnin"].as<uint>();

  std::vector<WorldParams> dumpParams;
  std::vector<double> dumpPrior;
  std::vector<double> dumpEnergy;
  std::vector<Eigen::VectorXd> dumpTheta;

  // Forward modelling stuff
  bool computeForwardModels = vm["fwdmodel"].as<bool>();
  std::vector<GlobalResults> dumpResults;
  std::vector<std::vector<double>> dumpLikelihood;

  std::string filename = vm["outputfile"].as<std::string>();
  io::NpzWriter writer(filename);
  bool quit = false;
  for (uint id = 0; id < chains.numTotalChains(); id+=chains.numChains())
  {
    // This is the coldest chain in a stack, so we can directly use its samples.
    for (uint i = burnin; i < chains.length(id); i += nthin + 1)
    {
      if(global::interruptedBySignal)
      {
        quit = true;
        break;
      }
      if(i%1000000==0) LOG(INFO)<< "reading state " << i << " of " << chains.length(id) << " in chain " << id;
      // get the state
      mcmc::State s = chains.state(id, i);
      GlobalParams params = prior.reconstruct(s.sample);
      // prior
      double priorLikelihood = prior.evaluate(s.sample);
      dumpPrior.push_back(priorLikelihood);
      // add the state for writing
      dumpParams.push_back(params.world);
      dumpEnergy.push_back(s.energy);
      dumpTheta.push_back(s.sample);

      if (computeForwardModels)
      {
        GlobalResults results = fwd::forwardModelAll(globalSpec, cache, params, sensorsEnabled);
        dumpResults.push_back(results);
        std::vector<double> likelihoods = lh::likelihoodAll(results, realResults, globalSpec, sensorsEnabled);
        dumpLikelihood.push_back(likelihoods); 
      }
    }
    if (quit)
      break;
  }

  dumpParamsNPZ(writer, dumpParams);
  dumpPriorNPZ(writer, dumpPrior);
  dumpEnergyNPZ(writer, dumpEnergy);
  dumpThetaNPZ(writer, dumpTheta);

  if (computeForwardModels)
  {
    dumpResultsNPZ(writer, dumpResults);
    dumpLikelihoodNPZ(writer, dumpLikelihood);
  }

  return 0;
}
