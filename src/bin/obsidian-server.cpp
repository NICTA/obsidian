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

//Stateline
#include <stateline/app/logging.hpp>
#include <stateline/app/serverwrapper.hpp>
#include <stateline/app/signal.hpp>
#include <stateline/app/commandline.hpp>

// Prerequisites
#include <glog/logging.h>
#include <boost/program_options.hpp>
#include <json.hpp>

// Standard Library
#include <thread>
#include <chrono>

namespace sl = stateline;
using json = nlohmann::json;

namespace ph = std::placeholders;

// Command line options specific to obsidian
po::options_description commandLineOptions()
{
  po::options_description cmdLine(" Command Line Options");
  cmdLine.add_options() //
  ("port,p", po::value<uint>()->default_value(5555), "TCP port to accept connections") //
  ("inputfile,i", po::value<std::string>()->default_value("input.obsidian"), "input file") //
  ("recover,r", po::bool_switch()->default_value(false), "force recovery")
  ("anneallength,a", po::value<uint>()->default_value(1000), "anneal chains with n samples before starting mcmc")
  ("loglevel,l", po::value<int>()->default_value(0), "Logging level")
  ("config,c",po::value<std::string>()->default_value("config.json"), "Path to configuration file")
  ;
  return cmdLine;
}
    
json initConfig(const po::variables_map& vm)
{
  std::ifstream configFile(vm["config"].as<std::string>());
  if (!configFile)
  {
    // TODO: use default settings?
    LOG(FATAL) << "Could not find config file";
  }

  json config;
  configFile >> config;
  return config;
}

int main(int ac, char* av[])
{
  // Get the settings from the command line
  auto vm = sl::parseCommandLine(ac, av, commandLineOptions());

  int logLevel = vm["loglevel"].as<int>();
  sl::initLogging("obsidian-server", logLevel);

  // Server
  json config = initConfig(vm);
  uint port = vm["port"].as<uint>();
  sl::StatelineSettings settings = sl::StatelineSettings::fromJSON(config);
  sl::ServerWrapper s(port, settings);
  s.start();

  while(!sl::global::interruptedBySignal)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  s.stop();

  return 0;
}
