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
bool forwardModelWorker( const GlobalSpec& gSpec,
                         const GlobalPrior& gPrior,
                         const GlobalResults& gResults,
                         const std::vector<world::InterpolatorSpec>& interp,
                         const po::variables_map &vm )
{
  LOG(INFO) << "Creating forwardModelWorker for " << f;

  const typename Types<f>::Spec& spec        = globalSpec<GlobalSpec,typename Types<f>::Spec>(gSpec);
  const typename Types<f>::Cache cache       = fwd::generateCache<f>(interp, gSpec.world, spec);
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

  const std::vector<std::string> jobTypes { configHeading<f>() };
  const uint nThreads = vm["nthreads"].as<uint>();
  const std::string address = vm["address"].as<std::string>();

  sl::WorkerWrapper w(lhFn, address, jobTypes, nThreads);

  w.start();

  while(!sl::global::interruptedBySignal)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  LOG(INFO) << "Stopping forwardModelWorker for " << f;

  w.stop();

  return true;
}

bool priorWorker( const GlobalPrior& gPrior,
                  const po::variables_map &vm )
{
  LOG(INFO) << "Creating prior model";

  sl::LikelihoodFn lhFn = [&](const std::string& jobType,
                              const std::vector<double>& sample) -> double
  {
    LOG(INFO) << "Received job type: " << jobType;
    Eigen::Map<const Eigen::VectorXd> theta(sample.data(),sample.size());
    auto p = gPrior.evaluate(theta);
    LOG(INFO) << "Evaluated Prior: " << p;
    return p;
  };

  const std::vector<std::string> jobTypes { "prior" };
  const uint nThreads = vm["nthreads"].as<uint>();
  const std::string address = vm["address"].as<std::string>();

  sl::WorkerWrapper w(lhFn, address, jobTypes, nThreads);

  w.start();

  while(!sl::global::interruptedBySignal){
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  LOG(INFO) << "Stopping prior model";

  w.stop();

  return true;
}


template<ForwardModel f>
struct launchWorkerThread
{
  launchWorkerThread( std::vector< std::future<bool> > & threads,
                      const GlobalSpec& gSpec,
                      const GlobalPrior& gPrior,
                      const GlobalResults& gResults,
                      const std::vector<world::InterpolatorSpec>& interp,
                      const po::variables_map &vm )
  {
    LOG(INFO)<< "Launching thread for " << f;
    threads.push_back(std::async(std::launch::async, forwardModelWorker<f>,
                                 std::cref(gSpec), std::cref(gPrior), std::cref(gResults),
                                 std::cref(interp), std::cref(vm))
                      );
  }
};


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
  const std::vector<world::InterpolatorSpec> interp = world::worldspec2Interp(gSpec.world);
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
  std::vector< std::future<bool> > threads;

  // Start prior worker thread
  threads.push_back(std::async(std::launch::async, priorWorker,
                               std::cref(gPrior), std::cref(vm) ) );

  // start all forward model worker threads
  applyToSensorsEnabled<launchWorkerThread>( enabled, std::ref(threads),
                                             std::cref(gSpec), std::cref(gPrior), std::cref(gResults),
                                             std::cref(interp), std::cref(vm) );

  while(!sl::global::interruptedBySignal) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  LOG(INFO)<< "Obsidian worker stopped: waiting for all threads";

  for (auto& t : threads) {
    t.wait();
  }

  LOG(INFO)<< "Obsidian worker exit";
  return 0;
}
