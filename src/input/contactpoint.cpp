//!
//! Input implementations related to contact point forward model.
//!
//! \file contactpoint.cpp
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"

namespace obsidian
{

  template<>
  std::string configHeading<ForwardModel::CONTACTPOINT>()
  {
    return "contactpoint";
  }

  template<>
  void initSensorInputFileOptions<ForwardModel::CONTACTPOINT>(po::options_description & options)
  {
    options.add_options() //
    ("contactpoint.enabled", po::value<bool>(), "enable sensor") //
    ("contactpoint.sensorLocations", po::value<std::string>(), "sensor locations") //
    ("contactpoint.sensorBoundaries", po::value<std::string>(), "boundaries touched by drill") //
    ("contactpoint.sensorReadings", po::value<std::string>(), "sensor readings") //
    ("contactpoint.noiseAlpha", po::value<double>(), "noise NIG alpha variable") //
    ("contactpoint.noiseBeta", po::value<double>(), "noise NIG beta variable");
  }

  template<>
  ContactPointSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    ContactPointSpec spec;
    if (sensorsEnabled.count(ForwardModel::CONTACTPOINT))
    {
      std::vector<std::vector<std::string>> data = io::csv::readRaw(vm["contactpoint.sensorLocations"].as<std::string>());
      spec.locations = readFixed<double, 2, 0>(data);
      spec.interfaces = readRagged<int, 2>(data);
      spec.noise.inverseGammaAlpha = vm["contactpoint.noiseAlpha"].as<double>();
      spec.noise.inverseGammaBeta = vm["contactpoint.noiseBeta"].as<double>();
    }
    return spec;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, ContactPointSpec spec, const po::options_description & od)
  {
    std::vector<std::vector<std::string>> data;
    writeFixed<double>(data, spec.locations);
    writeRagged<int>(data, spec.interfaces);
    io::csv::writeRaw(prefix + "sensorLocations.csv", data);

    return build_vm(po::variables_map(), od, "contactpoint",
                    { { "sensorLocations", prefix + "sensorLocations.csv" },
                      { "noiseAlpha", io::to_string(spec.noise.inverseGammaAlpha) },
                      { "noiseBeta", io::to_string(spec.noise.inverseGammaBeta) } });
  }

  //! @note the sensor params don't actually have anything in them at the moment so we don't need to do any parsing
  //!
  template<>
  ContactPointParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return ContactPointParams();
  }

  template<>
  ContactPointResults parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    ContactPointResults s;
    if (sensorsEnabled.count(ForwardModel::CONTACTPOINT))
    {
      std::vector<std::vector<std::string>> data = io::csv::readRaw(vm["contactpoint.sensorReadings"].as<std::string>());
      s.readings = readRagged<double, 0>(data);
      s.likelihood = 0.0;
    }
    return s;
  }
  template<>
  po::variables_map write<>(const std::string & prefix, ContactPointResults g, const po::options_description & od)
  {
    std::vector<std::vector<std::string>> data;
    writeRagged<double>(data, g.readings);
    io::csv::writeRaw(prefix + "sensorReadings.csv", data);
    return build_vm(po::variables_map(), od, "contactpoint", { { "sensorReadings", prefix + "sensorReadings.csv" } });
  }

  template<>
  void enableProperties<ForwardModel::CONTACTPOINT>(Eigen::VectorXi & propMasksMask)
  {
  }

  template<>
  prior::ContactPointParamsPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return prior::ContactPointParamsPrior();
  }

  template<>
  bool validateSensor(const WorldSpec & world, const ContactPointSpec & spec, const ContactPointResults & result)
  {
    bool valid = true;
    if (spec.locations.rows() == 0)
    {
      LOG(ERROR)<< "input: no contact point locations specified. Disable forward model if it is not used.";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != spec.interfaces.size())
    {
      LOG(ERROR)<< "input: different number of contact point interface (" << spec.interfaces.size() << ") to location specified. Remove or add more locations ("<< spec.interfaces.size() <<").";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.cols()) != 2)
    {
      LOG(ERROR)<< "input: locations in contact point must have two cols (x, y).";
      valid = false;
    }
    for (uint l = 0; l < spec.locations.rows(); l++)
    {
      if (spec.locations(l, 0) < world.xBounds.first || spec.locations(l, 0) > world.xBounds.second
          || spec.locations(l, 1) < world.yBounds.first || spec.locations(l, 1) > world.yBounds.second)
      {
        LOG(ERROR)<< "input: contact point location " << (l + 1) << " is out of world bounds";
        valid = false;
      }
    }
    for (uint l = 0; l < spec.interfaces.size(); l++)
    {
      if (spec.interfaces[l].rows() == 0)
      {
        LOG(ERROR)<< "input: contact point boundary " << (l + 1) << " does not have any layers. Remove sensor or add an interface.";
        valid = false;
      }
      for (uint i = 0; i < spec.interfaces[l].rows(); i++)
      {
        if (spec.interfaces[l][i] < 0 || spec.interfaces[l][i] >= static_cast<int>(world.boundaries.size()))
        {
          LOG(ERROR)<< "input: contact point boundary " << (l + 1) << " for layer " << (l + 1) << " must be between 0 and " << (world.boundaries.size() - 1);
          valid = false;
        }
      }
      if (spec.locations(l, 0) < world.xBounds.first || spec.locations(l, 0) > world.xBounds.second
          || spec.locations(l, 1) < world.yBounds.first || spec.locations(l, 1) > world.yBounds.second)
      {
        LOG(ERROR)<< "input: contact point location " << (l + 1) << " is out of world bounds";
        valid = false;
      }
    }
    if (spec.noise.inverseGammaAlpha <= 0 || spec.noise.inverseGammaAlpha <= 0)
    {
      LOG(ERROR)<< "input: contact point noise parameters must be greater than 0";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != result.readings.size())
    {
      LOG(ERROR)<< "input: different number of readings for contact point results (" << result.readings.size() << ") to location specified. Remove or add more locations ("<< spec.interfaces.size() <<").";
      valid = false;
    }
    for (uint l = 0; l < result.readings.size(); l++)
    {
      if (result.readings[l].rows() == 0)
      {
        LOG(ERROR)<< "input: contact point boundary " << (l + 1) << " readings are empty.";
        valid = false;
      }
      if (static_cast<uint>(result.readings[l].rows()) != spec.interfaces.size())
      {
        LOG(ERROR)<< "input: contact point boundary " << (l + 1) << " readings are should match the number of interfaces.";
        valid = false;
      }
    }
    return valid;
  }
}
