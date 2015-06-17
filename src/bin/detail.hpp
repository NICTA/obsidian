//!
//! Various helper methods to make code cleaner.
//!
//! \file detail.hpp
//! \author Nahid Akbar
//! \date 2014-06-17
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA.
//!

#pragma once

#include "datatype/sensors.hpp"

namespace obsidian
{
  template<ForwardModel f>
  struct helperGetJobType
  {
    helperGetJobType(std::vector<std::string> & allJobTypes)
    {
      allJobTypes.push_back(configHeading<f>());
    }
  };

  //! returns a list of job type name strings
  //!
  inline std::vector<std::string> getAllJobTypes()
  {
    std::vector<std::string> allJobTypes;
    applyToSensors<helperGetJobType>(std::ref(allJobTypes));
    return allJobTypes;
  }

  template<ForwardModel f>
  struct helperJobTypesToValues
  {
    helperJobTypesToValues(std::vector<uint> & jobList, std::vector<std::string> & jobTypes)
    {
      for (size_t i = 0; i < jobTypes.size(); i++)
      {
        if (configHeading<f>() == jobTypes[i])
        {
          jobTypes.erase(jobTypes.begin() + i);
          jobList.push_back(static_cast<uint>(f));
          break;;
        }
      }
    }
  };

  //! Parses a list of string job type and return their uint value
  //!
  inline std::vector<uint> jobTypesToValues(std::vector<std::string> jobTypes)
  {
    std::vector<uint> jobList;

    auto it = std::find(std::begin(jobTypes),std::end(jobTypes),"prior");
    if ( it != jobTypes.end() )
    {
      jobList.push_back( static_cast<uint>(ForwardModel::PRIOR) );
      jobTypes.erase(it);
    }

    applyToSensors<helperJobTypesToValues>(std::ref(jobList), std::ref(jobTypes));
    if (jobTypes.size() > 0)
    {
      for (std::string jt : jobTypes)
      {
        LOG(ERROR) << "Unknown Job Type specified " << jt;
      }
      exit(EXIT_FAILURE);
    }
    return jobList;
  }

  template<ForwardModel f>
  struct getResults
  {
    getResults(GlobalResults & globalResults, const WorldSpec & worldSpec, const po::variables_map &vm,
               const std::set<ForwardModel>& sensorsEnabled)
    {
      typename Types<f>::Results results;
      if (isSimulation(vm))
      {
        typename Types<f>::Spec spec = parseSpec<typename Types<f>::Spec>(vm, sensorsEnabled);
        LOG(WARNING) << "Simulation is on.";
        std::vector<world::InterpolatorSpec> layerInterp = world::worldspec2Interp(worldSpec);
        typename Types<f>::Cache cache = fwd::generateCache<f>(layerInterp, worldSpec, spec);
        WorldParams params = parseSimulationParams<WorldParams>(vm, sensorsEnabled);
        if (!validateWorld(worldSpec, params))
        {
          LOG(ERROR) << "Could not validate world model " << f << " and simulation params";
          exit(EXIT_FAILURE);
        }
        results = fwd::forwardModel<f>(spec, cache, params);
      } else
      {
        results = parseSensorReadings<typename Types<f>::Results>(vm, sensorsEnabled);
      }
      globalResult<GlobalResults, typename Types<f>::Results>(globalResults) = results;
    }
  };

  inline GlobalResults loadResults(const WorldSpec & worldSpec, const po::variables_map &vm, const std::set<ForwardModel>& sensorsEnabled)
  {
    LOG(INFO) << "Loading sensor data";
    GlobalResults results;
    applyToSensorsEnabled<getResults>(sensorsEnabled, std::ref(results), std::cref(worldSpec), std::cref(vm), std::cref(sensorsEnabled));
    return results;
  }

}
