//!
//! Input implementations related to gravity forward model.
//!
//! \file gravity.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"

namespace obsidian
{

  template<>
  std::string configHeading<ForwardModel::GRAVITY>()
  {
    return "gravity";
  }

  template<>
  void initSensorInputFileOptions<ForwardModel::GRAVITY>(po::options_description & options)
  {
    options.add_options() //
    ("gravity.enabled", po::value<bool>(), "enable sensor") //
    ("gravity.sensorLocations", po::value<std::string>(), "sensor locations") //
    ("gravity.sensorReadings", po::value<std::string>(), "sensor readings") //
    ("gravity.gridResolution", po::value<Eigen::Vector3i>(), "grid points per cube side") //
    ("gravity.noiseAlpha", po::value<double>(), "noise NIG alpha variable") //
    ("gravity.noiseBeta", po::value<double>(), "noise NIG beta variable") //
    ("gravity.supersample", po::value<uint>(), "supersampling exponent");
  }

  template<>
  GravSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    GravSpec spec;
    if (sensorsEnabled.count(ForwardModel::GRAVITY))
    {
      // Gravity
      LOG(INFO)<< "Initialising Gravity data";
      spec.locations = io::csv::read<double>(vm["gravity.sensorLocations"].as<std::string>());
      Eigen::Vector3i gravRes = vm["gravity.gridResolution"].as<Eigen::Vector3i>();
      spec.voxelisation.xResolution = uint(gravRes(0));
      spec.voxelisation.yResolution = uint(gravRes(1));
      spec.voxelisation.zResolution = uint(gravRes(2));
      spec.voxelisation.supersample = vm["gravity.supersample"].as<uint>();
      spec.noise.inverseGammaAlpha = vm["gravity.noiseAlpha"].as<double>();
      spec.noise.inverseGammaBeta = vm["gravity.noiseBeta"].as<double>();
    }
    return spec;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, GravSpec spec, const po::options_description & od)
  {
    io::csv::write<double>(prefix + "sensorLocations.csv", spec.locations);
    return build_vm(
        po::variables_map(),
        od,
        "gravity",
        { { "sensorLocations", prefix + "sensorLocations.csv" },
          { "gridResolution", io::to_string(spec.voxelisation.xResolution, spec.voxelisation.yResolution, spec.voxelisation.zResolution) },
          { "supersample", io::to_string(spec.voxelisation.supersample) },
          { "noiseAlpha", io::to_string(spec.noise.inverseGammaAlpha) },
          { "noiseBeta", io::to_string(spec.noise.inverseGammaBeta) } });
  }

  //! @note the sensor params don't actually have anything in them at the moment so we don't need to do any parsing
  //!
  template<>
  GravParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return GravParams();
  }

  template<>
  GravResults parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    GravResults s;
    if (sensorsEnabled.count(ForwardModel::GRAVITY))
    {
      s.readings = io::csv::read<double, Eigen::Dynamic, 1>(vm["gravity.sensorReadings"].as<std::string>());
      s.likelihood = 0.0;
    }
    return s;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, GravResults g, const po::options_description & od)
  {
    io::csv::write<double, Eigen::Dynamic, 1>(prefix + "sensorReadings.csv", g.readings);
    return build_vm(po::variables_map(), od, "gravity", { { "sensorReadings", prefix + "sensorReadings.csv" } });
  }

  template<>
  void enableProperties<ForwardModel::GRAVITY>(Eigen::VectorXi & propMasksMask)
  {
    propMasksMask(static_cast<uint>(RockProperty::Density)) = 1;
  }

  template<>
  prior::GravParamsPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return prior::GravParamsPrior();
  }

  template<>
  bool validateSensor(const WorldSpec & world, const GravSpec & spec, const GravResults & result)
  {
    bool valid = true;
    if (spec.locations.rows() == 0)
    {
      LOG(ERROR)<< "input: no gravity locations specified. Disable forward model if it is not used.";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.cols()) != 3)
    {
      LOG(ERROR)<< "input: locations in gravity must have three cols (x, y, z).";
      valid = false;
    }
    for (uint l = 0; l < spec.locations.rows(); l++)
    {
      if (spec.locations(l, 0) < world.xBounds.first || spec.locations(l, 0) > world.xBounds.second
          || spec.locations(l, 1) < world.yBounds.first || spec.locations(l, 1) > world.yBounds.second)

      {
        LOG(ERROR)<< "input: gravity location " << (l + 1) << " is out of world bounds";
        valid = false;
      }
    }
    if (spec.voxelisation.xResolution <= 0 || spec.voxelisation.yResolution <= 0 || spec.voxelisation.zResolution <= 0)
    {
      LOG(ERROR)<< "input: gravity voxelisation (x, y, z)must be greater than 0.";
      valid = false;
    }
    if (spec.noise.inverseGammaAlpha <= 0 || spec.noise.inverseGammaAlpha <= 0)
    {
      LOG(ERROR)<< "input: gravity noise parameters must be greater than 0";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != result.readings.size())
    {
      LOG(ERROR)<< "input: different number of readings for gravity results (" << result.readings.size() << ") to location specified. Remove or add more locations ("<< spec.locations.rows() <<").";
      valid = false;
    }
    return valid;
  }
}
