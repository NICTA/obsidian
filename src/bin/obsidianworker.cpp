//!
//! Runs stateline worker and obsidian fwd model minions
//!
//! \file bin/obsidianworker.cpp
//! \author Dave Cole
//! \date 2015
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!

#include "obsidianworker.hpp"
#include "detail.hpp"

#include <stateline/comms/minion.hpp>
#include <stateline/comms/worker.hpp>


namespace obsidian
{

void runWorker(zmq::context_t& context, const std::string& address, bool& running)
{
  LOG(INFO) << "Creating worker";
  auto settings = sl::comms::WorkerSettings::Default(address);
  sl::comms::Worker worker(context, settings, running);
  worker.start();
  LOG(INFO) << "Worker started";
}

void runMinion(const LikelihoodFn& f, zmq::context_t& context,
               const std::vector<std::string>& jobTypes, bool& running)
{
  LOG(INFO) << "Creating minion for " << jobTypes[0];
  sl::comms::Minion minion(context, jobTypes);
  while (running)
  {
    try {
      auto job = minion.nextJob();
      auto jobType = job.first;
      auto sample = job.second;
      minion.submitResult(f(jobType, sample));
    }
    catch (...)
    {
      LOG(INFO) << "Job " << jobTypes[0] << " failed ";
    }
  }
}

template<ForwardModel f>
void runFwdModelMinions(zmq::context_t& context,
                        bool& running,
                        uint nThreads,
                        const GlobalSpec& gSpec,
                        const GlobalPrior& gPrior,
                        const GlobalResults& gResults,
                        const std::vector<world::InterpolatorSpec>& interp)
{
  LOG(INFO) << "Creating " << nThreads << " forwardModelMinion(s) for " << f;
  const typename Types<f>::Spec& spec        = globalSpec<GlobalSpec,typename Types<f>::Spec>(gSpec);
  const typename Types<f>::Cache cache       = fwd::generateCache<f>(interp, gSpec.world, spec);
  const typename Types<f>::Results& results  = globalResult<GlobalResults, typename Types<f>::Results>(gResults);

  LikelihoodFn lhFn = [&](const std::string& jobType, const std::vector<double>& sample) -> double
  {
    Eigen::Map<const Eigen::VectorXd> theta( sample.data(), sample.size() );
    const GlobalParams gParams = gPrior.reconstruct(theta);
    typename Types<f>::Results synthetic = fwd::forwardModel<f>(spec, cache, gParams.world);
    synthetic.likelihood = lh::likelihood<f>(synthetic, results, spec);
    LOG_EVERY_N(INFO,10) << "Evaluated " << f << " likelihhood function (" << jobType << "): " << synthetic.likelihood;
    return synthetic.likelihood;
  };

  const std::vector<std::string> jobTypes { configHeading<f>() };

  std::vector< std::future<void> > minionThreads;
  for (uint i=0; i<nThreads;i++)
  {
    minionThreads.push_back(
      std::async( std::launch::async, runMinion,
                  std::cref(lhFn), std::ref(context),
                  std::cref(jobTypes), std::ref(running) )
    );
  }

  for ( auto& t : minionThreads ) {
    t.wait();
  }
  return;
}

void runPriorMinion(zmq::context_t& context,
                    bool& running,
                    uint nThreads,
                    const GlobalPrior& gPrior)
{
  LOG(INFO) << "Creating " << nThreads << " priorMinions(s)";

  LikelihoodFn lhFn = [&](const std::string& jobType, const std::vector<double>& sample) -> double
  {
    Eigen::Map<const Eigen::VectorXd> theta(sample.data(),sample.size());
    auto p = gPrior.evaluate(theta);
    LOG_EVERY_N(INFO,10) << "Evaluated Prior (" << jobType << "): " << p;
    return p;
  };

  const std::vector<std::string> jobTypes { "prior" };

  std::vector< std::future<void> > minionThreads;
  for (uint i=0; i<nThreads;i++)
  {
    minionThreads.push_back(
      std::async( std::launch::async, runMinion,
                  std::cref(lhFn), std::ref(context),
                  std::cref(jobTypes), std::ref(running) )
    );
  }

  for ( auto& t : minionThreads ) {
    t.wait();
  }
  return;
}


template<ForwardModel f>
struct launchForwardModelThread
{
  launchForwardModelThread( std::vector< std::future<void> > &threads,
                            zmq::context_t& context,
                            bool& running,
                            uint nThreads,
                            const GlobalSpec& gSpec,
                            const GlobalPrior& gPrior,
                            const GlobalResults& gResults,
                            const std::vector<world::InterpolatorSpec>& interp)
  {
    LOG(INFO)<< "Launching thread for " << f;
    threads.push_back(std::async(std::launch::async, runFwdModelMinions<f>,
                                 std::ref(context), std::ref(running), nThreads,
                                 std::cref(gSpec), std::cref(gPrior),
                                 std::cref(gResults), std::cref(interp) ) );
  }
};

ObsidianWorker::ObsidianWorker(const GlobalSpec& gSpec, const GlobalPrior& gPrior, const GlobalResults& gResults,
                               const std::string& address, const std::vector<std::string>& jobTypes, uint nThreads)
  : gSpec_(gSpec), gPrior_(gPrior), gResults_(gResults)
  , interp_(world::worldspec2Interp(gSpec.world))
  , address_(address), jobTypes_(jobTypes), nThreads_(nThreads)
{
}

void ObsidianWorker::start()
{
  LOG(INFO)<< "Starting ObsidianWorker";
  context_ = new zmq::context_t{1};
  running_ = true;


  LOG(INFO)<< "Launching thread for prior";
  modelThreads_.push_back(std::async(std::launch::async, runPriorMinion,
                               std::ref(*context_), std::ref(running_), nThreads_,
                               std::cref(gPrior_) ) );


  std::vector<uint> jobList = jobTypesToValues(jobTypes_);
  std::set<ForwardModel> enabled;
  for (uint i : jobList) {
    enabled.insert(static_cast<ForwardModel>(i));
  }
  applyToSensorsEnabled<launchForwardModelThread>( enabled, std::ref(modelThreads_), std::ref(*context_), std::ref(running_), nThreads_,
                                                   std::cref(gSpec_), std::cref(gPrior_), std::cref(gResults_), std::cref(interp_) );

  std::this_thread::sleep_for(std::chrono::milliseconds(15000));

  workerThread_ = std::async(std::launch::async, runWorker, std::ref(*context_), std::cref(address_), std::ref(running_));

}

void ObsidianWorker::stop()
{
  running_ = false;
  LOG(INFO)<< "Delete zmq context";
  if (context_)
  {
    delete context_;
    context_ = nullptr; //THIS MUST BE DONE
  }
  LOG(INFO)<< "Waiting on worker thread";
  workerThread_.wait();

  for (auto const& t : modelThreads_)
  {
    LOG(INFO)<< "Waiting on model threads";
    t.wait();
  }

}

ObsidianWorker::~ObsidianWorker()
{
  stop();
}


}
