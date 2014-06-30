//!
//! Input implementations related to thermal forward model.
//!
//! \file thermal.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"

namespace obsidian
{
  template<>
  std::string configHeading<ForwardModel::THERMAL>()
  {
    return "thermal";
  }

  template<>
  void initSensorInputFileOptions<ForwardModel::THERMAL>(po::options_description & options)
  {
    options.add_options() //
    ("thermal.enabled", po::value<bool>(), "enable sensor") //
    ("thermal.sensorLocations", po::value<std::string>(), "Thermal sensor locations") //
    ("thermal.sensorReadings", po::value<std::string>(), "Thermal sensor readings") //
    ("thermal.surfaceTemperature", po::value<double>(), "Surface temperature") //
    ("thermal.lowerBoundary", po::value<double>(), "Bottom boundary condition") //
    ("thermal.lowerBoundaryIsHeatFlow", po::value<bool>(), "Interpet boundary as heat not temperature") //
    ("thermal.gridResolution", po::value<Eigen::Vector3i>(), "grid points per cube side") //
    ("thermal.supersample", po::value<uint>(), "supersampling exponent") //
    ("thermal.noiseAlpha", po::value<double>(), "noise NIG alpha variable") //
    ("thermal.noiseBeta", po::value<double>(), "noise NIG beta variable");
  }

  template<>
  ThermalSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    ThermalSpec spec;
    if (sensorsEnabled.count(ForwardModel::THERMAL))
    {
      spec.locations = io::csv::read<double>(vm["thermal.sensorLocations"].as<std::string>());
      Eigen::Vector3i thermRes = vm["thermal.gridResolution"].as<Eigen::Vector3i>();
      spec.surfaceTemperature = vm["thermal.surfaceTemperature"].as<double>();
      spec.lowerBoundary = vm["thermal.lowerBoundary"].as<double>();
      spec.lowerBoundaryIsHeatFlow = vm["thermal.lowerBoundaryIsHeatFlow"].as<bool>();
      spec.voxelisation.xResolution = uint(thermRes(0));
      spec.voxelisation.yResolution = uint(thermRes(1));
      spec.voxelisation.zResolution = uint(thermRes(2));
      spec.voxelisation.supersample = vm["thermal.supersample"].as<uint>();
      spec.noise.inverseGammaAlpha = vm["thermal.noiseAlpha"].as<double>();
      spec.noise.inverseGammaBeta = vm["thermal.noiseBeta"].as<double>();
    }
    return spec;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, ThermalSpec spec, const po::options_description & od)
  {
    io::csv::write<double>(prefix + "sensorLocations.csv", spec.locations);
    return build_vm(
        po::variables_map(), od, "thermal",
        { { "sensorLocations", prefix + "sensorLocations.csv" },
          { "surfaceTemperature", io::to_string(spec.surfaceTemperature) },
          { "lowerBoundary", io::to_string(spec.lowerBoundary) },
          { "lowerBoundaryIsHeatFlow", io::to_string(spec.lowerBoundaryIsHeatFlow) },
          { "gridResolution", io::to_string(spec.voxelisation.xResolution, spec.voxelisation.yResolution, spec.voxelisation.zResolution) },
          { "supersample", io::to_string(spec.voxelisation.supersample) },
          { "noiseAlpha", io::to_string(spec.noise.inverseGammaAlpha) },
          { "noiseBeta", io::to_string(spec.noise.inverseGammaBeta) } });
  }

  //! @note the sensor params don't actually have anything in them at the moment so we don't need to do any parsing
  //!
  template<>
  ThermalParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return ThermalParams();
  }
  template<>
  ThermalResults parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    ThermalResults s;
    if (sensorsEnabled.count(ForwardModel::THERMAL))
    {
      s.readings = io::csv::read<double, Eigen::Dynamic, 1>(vm["thermal.sensorReadings"].as<std::string>());
      s.likelihood = 0.0;
    }
    return s;
  }
  template<>
  po::variables_map write<>(const std::string & prefix, ThermalResults g, const po::options_description & od)
  {
    io::csv::write<double, Eigen::Dynamic, 1>(prefix + "sensorReadings.csv", g.readings);
    return build_vm(po::variables_map(), od, "thermal", { { "sensorReadings", prefix + "sensorReadings.csv" } });
  }

  template<>
  void enableProperties<ForwardModel::THERMAL>(Eigen::VectorXi & propMasksMask)
  {
    propMasksMask(static_cast<uint>(RockProperty::ThermalConductivity)) = 1;
    propMasksMask(static_cast<uint>(RockProperty::ThermalProductivity)) = 1;
  }

  template<>
  prior::ThermalParamsPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return prior::ThermalParamsPrior();
  }

  template<>
  bool validateSensor(const WorldSpec & world, const ThermalSpec & spec, const ThermalResults & result)
  {
    bool valid = true;
    if (spec.locations.rows() == 0)
    {
      LOG(ERROR)<< "input: no thermal locations specified. Disable forward model if it is not used.";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.cols()) != 3)
    {
      LOG(ERROR)<< "input: locations in thermal must have three cols (x, y, z).";
      valid = false;
    }
    for (uint l = 0; l < spec.locations.rows(); l++)
    {
      if (spec.locations(l, 0) < world.xBounds.first || spec.locations(l, 0) > world.xBounds.second
          || spec.locations(l, 1) < world.yBounds.first || spec.locations(l, 1) > world.yBounds.second
          || spec.locations(l, 2) < world.zBounds.first || spec.locations(l, 2) > world.zBounds.second)
      {
        LOG(ERROR)<< "input: thermal location " << (l + 1) << " is out of world bounds";
        valid = false;
      }
    }
    if (!spec.lowerBoundaryIsHeatFlow)
    {
      if (spec.lowerBoundary <= 0)
      {
        LOG(ERROR)<< "input: thermal lower boundary temperature should be greater than zero (kelvins).";
        valid = false;
      }
    }
    if (spec.surfaceTemperature <= 0)
    {
      LOG(ERROR)<< "input: thermal surface temperature should be greater than zero (kelvins).";
      valid = false;
    }
    if (spec.voxelisation.xResolution <= 0 || spec.voxelisation.yResolution <= 0 || spec.voxelisation.zResolution <= 0)
    {
      LOG(ERROR)<< "input: thermal voxelisation (x, y, z)must be greater than 0.";
      valid = false;
    }
    if (spec.noise.inverseGammaAlpha <= 0 || spec.noise.inverseGammaAlpha <= 0)
    {
      LOG(ERROR)<< "input: thermal noise parameters must be greater than 0";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != result.readings.size())
    {
      LOG(ERROR)<< "input: different number of readings for thermal results (" << result.readings.size() << ") to location specified. Remove or add more locations ("<< spec.locations.rows() <<").";
      valid = false;
    }
    return valid;
  }
}
