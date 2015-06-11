//!
//! Runs stateline worker and obsidian fwd model minions
//!
//!
//! \file bin/obsidianworker.hpp
//! \author Dave Cole
//! \date 2015
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!

#include "input/input.hpp"

//#include "datatype/sensors.hpp"
//#include "fwdmodel/fwd.hpp"
#include "likelihood/likelihood.hpp"
//#include "serial/serial.hpp"

#include <future>
#include <zmq.hpp>

namespace sl = stateline;

namespace obsidian
{
  typedef std::function<double(const std::string&, const std::vector<double>&)> LikelihoodFn;

  class ObsidianWorker
  {

    public:
      ObsidianWorker(const GlobalSpec& gSpec, const GlobalPrior& gPrior, const GlobalResults& gResults,
                     const std::string& address, const std::vector<std::string>& jobTypes, uint nThreads);
      ~ObsidianWorker();
      void start();
      void stop();
      bool isRunning() const { return running_; }

    private:

      template<ForwardModel f>
      void runFwdModelMinions();

      const GlobalSpec& gSpec_;
      const GlobalPrior& gPrior_;
      const GlobalResults& gResults_;
      const std::vector<world::InterpolatorSpec> interp_;

      std::string address_;
      std::vector<std::string> jobTypes_;
      uint nThreads_;

      bool running_;
      zmq::context_t* context_;
      std::future<void> workerThread_;
      std::vector<std::future<void>> modelThreads_;
  };

}
