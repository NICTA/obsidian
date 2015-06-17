//!
//! A worker application for executing Obsidian job types
//!
//! \file obsidian-worker.cpp
//! \author Dave Cole
//! \date 2015
//! \licence Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!

#include "input/input.hpp"
#include "datatype/sensors.hpp"
#include "fwdmodel/global.hpp"
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
    ("nworkers,w",  po::value<uint>()->default_value(1), "Number of worker threads")
    ("address,a",   po::value<std::string>()->default_value("localhost:5555"), "Address of server")
    ("input,i",     po::value<std::string>()->default_value("input.obsidian"), "Input file")
    ("jobtypes,j",  po::value<std::string>()->default_value("prior"), "Comma-separated job types")
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


template<ForwardModel f>
struct generateLikelihoodFn
{
  generateLikelihoodFn( sl::JobLikelihoodFnMap& lhMap,
                        const GlobalSpec& gSpec,
                        const GlobalPrior& gPrior,
                        const GlobalResults& gResults,
                        const GlobalCache& gCache)
  {
    const typename Types<f>::Spec& spec        = globalSpec<GlobalSpec,typename Types<f>::Spec>(gSpec);
    const typename Types<f>::Cache& cache      = globalCache<GlobalCache,typename Types<f>::Cache>(gCache);
    const typename Types<f>::Results& results  = globalResult<GlobalResults, typename Types<f>::Results>(gResults);

    sl::LikelihoodFn lhFn = [&](const std::string& jobType,
                                const std::vector<double>& sample) -> double
    {
      Eigen::Map<const Eigen::VectorXd> theta(sample.data(),sample.size());
      const GlobalParams gParams = gPrior.reconstruct(theta);
      typename Types<f>::Results synthetic = fwd::forwardModel<f>(spec, cache, gParams.world);
      synthetic.likelihood = lh::likelihood<f>(synthetic, results, spec);
      LOG(INFO) << "Evaluated " << f << " likelihhood function (" << jobType << "): " << synthetic.likelihood;
      return synthetic.likelihood;
    };

    const auto& jobStr = configHeading<f>();
    lhMap[jobStr] = lhFn;
  }
};


void runWorker(const sl::JobLikelihoodFnMap& lhMap, const std::string& address)
{
  sl::WorkerWrapper w(lhMap, address);
  w.start();

  while(!sl::global::interruptedBySignal)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  w.stop();
}

std::string to_string(std::set<ForwardModel> enabled)
{
  std::ostringstream oss;
  std::string sep;
  for ( auto& m : enabled )
  {
    oss << sep << m;
    sep = ",";
  }
  return oss.str();
}


int main(int ac, char *av[])
{
  LOG(INFO)<< "Running obsidian-worker";

  auto vm = sl::parseCommandLine(ac, av, commandLineOptions());

  const int logLevel = vm["loglevel"].as<int>();
  sl::initLogging("obsidian-worker", logLevel);

  sl::init::initialiseSignalHandler();

  readInputFile(vm["input"].as<std::string>(), vm);

  // Determine which sensors are enabled from config and command line overrides
  std::set<ForwardModel> modelsConfig = parseSensorsEnabled(vm);
  modelsConfig.insert(ForwardModel::PRIOR);

  std::vector<uint> jobList = jobTypesToValues(io::split(vm["jobtypes"].as<std::string>(), ','));

  std::set<ForwardModel> modelsEnabled;
  if ( !jobList.empty() ) {
    std::set<ForwardModel> modelsOverride;
    for (uint i : jobList) {
      modelsOverride.insert(static_cast<ForwardModel>(i));
    }
    set_intersection( modelsConfig.begin(), modelsConfig.end(),
                      modelsOverride.begin(), modelsOverride.end(),
                      std::inserter(modelsEnabled, modelsEnabled.begin()) );
  } else {
    modelsEnabled = modelsConfig;
  }

  // Create all data structures required for forward modelling
  const GlobalSpec gSpec = parseSpec<GlobalSpec>(vm, modelsEnabled);
  const GlobalPrior gPrior = parsePrior<GlobalPrior>(vm, modelsEnabled);
  const GlobalResults gResults = loadResults(gSpec.world, vm, modelsEnabled);
  const std::vector<world::InterpolatorSpec> interp = world::worldspec2Interp(gSpec.world);
  const GlobalCache gCache = fwd::generateGlobalCache(interp, gSpec, modelsEnabled);

  applyToSensorsEnabled<validateSensorData>( modelsEnabled, std::cref(gSpec), std::cref(gResults) );

  // Prior model function
  sl::LikelihoodFn priorFn = [&](const std::string& jobType,
                                 const std::vector<double>& sample) -> double
  {
    Eigen::Map<const Eigen::VectorXd> theta(sample.data(),sample.size());
    auto p = gPrior.evaluate(theta);
    LOG(INFO) << "Evaluated Prior (" << jobType << "): " << p;
    return p;
  };

  sl::JobLikelihoodFnMap lhMap;
  if ( modelsEnabled.count(ForwardModel::PRIOR) )
  {
    lhMap["prior"] = priorFn;
  }

  // Add forward model likelihood functions
  applyToSensorsEnabled<generateLikelihoodFn>( modelsEnabled, std::ref(lhMap),
                                               std::cref(gSpec), std::cref(gPrior),
                                               std::cref(gResults), std::cref(gCache) );

  const std::string address = vm["address"].as<std::string>();
  const uint nWorkers = vm["nworkers"].as<uint>();

  // Start worker threads
  std::vector< std::future<void> > threads;
  for (uint i=0; i<nWorkers; ++i)
  {
    threads.push_back( std::async( std::launch::async, runWorker,
                                   std::cref(lhMap), std::cref(address) ) );
  }

  for (auto& t : threads) {
    t.wait();
  }

  LOG(INFO)<< "Obsidian worker exit";
  return 0;
}
