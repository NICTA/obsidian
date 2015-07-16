//!
//! An application for drawing samples directly from the prior
//!
//! \file prior-sampler.cpp
//! \author Dave Cole
//! \date 2015
//! \licence Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!

#include "input/input.hpp"
#include "detail.hpp"

#include <stateline/app/logging.hpp>
#include <stateline/app/signal.hpp>
#include <stateline/app/commandline.hpp>

#include <boost/program_options.hpp>

#include <fstream>

using namespace obsidian;
namespace sl = stateline;
namespace po = boost::program_options;

po::options_description commandLineOptions()
{
  auto opts = po::options_description("Obsidian prior sampler options");
  opts.add_options()
    ("loglevel,l",     po::value<int>()->default_value(0), "Logging level")
    ("input,i",        po::value<std::string>()->default_value("obsidian.input"), "Input file")
    ("nsamples,n",     po::value<uint>()->default_value(0), "Number of random samples to draw")
    ("energy,e",       po::bool_switch(), "Calculate and store the likelihood")
    ("output,o",       po::value<std::string>()->default_value("prior-samples.csv"), "CSV output file")
    ("append-dummy,d", po::value<uint>()->default_value(0), "Number of dummy values to add to end of each sample.")
    ;
  return opts;
}

int main(int ac, char *av[])
{
  auto vm = sl::parseCommandLine(ac, av, commandLineOptions());

  const int logLevel = vm["loglevel"].as<int>();
  sl::initLogging("prior-sampler", logLevel);

  sl::init::initialiseSignalHandler();

  readInputFile(vm["input"].as<std::string>(), vm);

  // Determine which sensors are enabled
  std::set<ForwardModel> modelsConfig = parseSensorsEnabled(vm);

  // Create world model prior
  const auto wPrior = parsePrior<prior::WorldParamsPrior>(vm, modelsConfig);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::string outfile = vm["output"].as<std::string>();
  std::ofstream ofs(outfile);

  const uint d = vm["append-dummy"].as<uint>();
  std::ostringstream oss;
  for (uint i = 0; i < d; ++i )
  {
    oss << ",0";
  }
  const std::string dummy = oss.str();

  const bool energy = vm["energy"].as<bool>();
  const uint n = vm["nsamples"].as<uint>();
  Eigen::IOFormat csv(6, Eigen::DontAlignCols, ",");
  for ( uint i = 0; i < n; ++i )
  {
    Eigen::VectorXd theta = wPrior.sample(gen);
    ofs << theta.transpose().format(csv);
    if (energy)
    {
      ofs << "," << -1.0 * wPrior.evaluatePDF(theta);
    }
    ofs << dummy << std::endl;
  }

  return 0;
}
