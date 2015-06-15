//!
//! A worker application for executing Obsidian job types
//!
//! \file obsidian-worker.cpp
//! \author Dave Cole
//! \date 2015
//! \licence Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!
#include "obsidianworker.hpp"
#include "input/input.hpp"
#include "datatype/sensors.hpp"
#include "fwdmodel/fwd.hpp"
#include "likelihood/likelihood.hpp"
#include "detail.hpp"
#include "serial/serial.hpp"
#include "io/string.hpp"

#include <stateline/app/logging.hpp>
#include <stateline/app/signal.hpp>
#include <stateline/app/commandline.hpp>
#include <stateline/app/workerwrapper.hpp>

#include <boost/program_options.hpp>

#include <string>
#include <thread>

using namespace obsidian;
namespace sl = stateline;
namespace po = boost::program_options;

po::options_description commandLineOptions()
{
  auto opts = po::options_description("Obsidian worker options");
  opts.add_options()
    ("loglevel,l",  po::value<int>()->default_value(0), "Logging level")
    ("nthreads,t",  po::value<uint>()->default_value(1), "Number of worker threads")
    ("address,a",   po::value<std::string>()->default_value("localhost:5555"), "Address of server")
    ("input,i",     po::value<std::string>()->default_value("input.obsidian"), "Input file")
    ("jobtypes,j",  po::value<std::string>()->default_value("gravity"), "Comma-separated job types")
    ;
  return opts;
}

template<ForwardModel f>
struct validateSensorData
{
  validateSensorData(const GlobalSpec& gSpec, const GlobalResults& gResults)
  {
    const WorldSpec wSpec = gSpec.world;
    const typename Types<f>::Spec& spec        = globalSpec<GlobalSpec,typename Types<f>::Spec>(gSpec);
    const typename Types<f>::Results& results  = globalResult<GlobalResults, typename Types<f>::Results>(gResults);

    if ( !validateSensor(wSpec, spec, results) )
    {
      LOG(ERROR) << "Could note validate " << f << " data";
      exit(EXIT_FAILURE);
    }
  }
};


int main(int ac, char *av[])
{
  LOG(INFO)<< "Running obsidian-worker";

  auto vm = sl::parseCommandLine(ac, av, commandLineOptions());

  int logLevel = vm["loglevel"].as<int>();
  sl::initLogging("obsidian-worker", logLevel);

  sl::init::initialiseSignalHandler();

  readInputFile(vm["input"].as<std::string>(), vm);

  const std::set<ForwardModel> sensorsEnabled = parseSensorsEnabled(vm);
  const GlobalSpec gSpec = parseSpec<GlobalSpec>(vm, sensorsEnabled);
  const GlobalPrior gPrior = parsePrior<GlobalPrior>(vm, sensorsEnabled);
  const GlobalResults gResults = loadResults(gSpec.world, vm, sensorsEnabled);

  std::vector<uint> jobList = jobTypesToValues(io::split(vm["jobtypes"].as<std::string>(), ','));
  std::set<ForwardModel> enabled;
  for (uint i : jobList) {
    enabled.insert(static_cast<ForwardModel>(i));
  }

  // validate
  LOG(INFO)<< "Validating data";
  applyToSensorsEnabled<validateSensorData>( enabled, std::cref(gSpec), std::cref(gResults) );

  // start forward-model workers
  LOG(INFO)<< "Starting all worker threads";
  const uint nThreads = vm["nthreads"].as<uint>();
  const std::string address = vm["address"].as<std::string>();
  const auto jobTypes = io::split(vm["jobtypes"].as<std::string>(), ',');

  ObsidianWorker w( gSpec, gPrior, gResults, address, jobTypes, nThreads );

  w.start();

  LOG(INFO)<< "Worker threads started";

  while(!sl::global::interruptedBySignal){
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  LOG(INFO)<< "Obsidian client shutting down";

  w.stop();

  LOG(INFO)<< "Obsidian client exit";
  return 0;
}
