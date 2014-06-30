//!
//! Input implementations.
//!
//! \file input.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <fstream>
#include <Eigen/Dense>
#include "input/input.hpp"
#include "io/string.hpp"
#include "io/npy.hpp"
#include "distrib/multigaussian.hpp"
#include "common.hpp"

namespace obsidian
{
  Eigen::VectorXd drawControlPoints(uint n)
  {
    LOG(INFO)<< "Sampling control points...";
    Eigen::VectorXd m(n);
    m.setRandom();
    return m;
  }

  template<ForwardModel f>
  struct initSensorInputFileOptions_
  {
    initSensorInputFileOptions_(po::options_description &options)
    {
      initSensorInputFileOptions<f>(options);
    }
  };

  po::options_description inputFileOptions()
  {
    po::options_description inputFile("Input File Options");
    initWorldOptions(inputFile);
    initMCMCOptions(inputFile);
    applyToSensors<initSensorInputFileOptions_>(std::ref(inputFile));
    return inputFile;
  }

  void readInputFile(const std::string& inputFilename, po::variables_map& vm)
  {
    // Create the global options object
    po::options_description inputFile("Input Options");
    inputFile.add(inputFileOptions());
    std::ifstream ifsInp(inputFilename);
    LOG(INFO)<< "Reading input from " << inputFilename;
    if (!ifsInp.fail())
    {
      po::store(po::parse_config_file(ifsInp, inputFile), vm);
      po::notify(vm);
    } else
    {
      LOG(ERROR)<< "Input file " << inputFilename << " could not be opened\n";
      exit (EXIT_FAILURE);
    }
  }

  template<ForwardModel f>
  struct parseSensorsEnabled_
  {
    parseSensorsEnabled_(std::set<ForwardModel> &enabled, const po::variables_map &vm)
    {
      std::string property_name = (configHeading<f>() + ".enabled").c_str();
//      printf("%s\n", property_name.c_str());
      if (vm[property_name.c_str()].as<bool>())
      {
        enabled.insert(f);
      }
    }
  };

  std::set<ForwardModel> parseSensorsEnabled(const po::variables_map& vm)
  {
    std::set<ForwardModel> enabled;
    applyToSensors<parseSensorsEnabled_>(std::ref(enabled), std::cref(vm));
    return enabled;
  }

  bool isSimulation(const po::variables_map& vm)
  {
    uint len = io::split(vm["simulation.ctrlPoints"].as<std::string>()).size();
    return len > 0;
  }

}
