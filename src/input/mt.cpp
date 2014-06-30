//!
//! Input implementations related to mt aniso forward model.
//!
//! \file mt.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"
#include "fwdmodel/mt1d.hpp"

namespace obsidian
{
  template<>
  std::string configHeading<ForwardModel::MTANISO>()
  {
    return "mtaniso";
  }

  template<>
  void initSensorInputFileOptions<ForwardModel::MTANISO>(po::options_description & options)
  {
    options.add_options() //
    ("mtaniso.enabled", po::value<bool>(), "enable sensor") //
    ("mtaniso.sensorLocations", po::value<std::string>(), "MT sensor locations and frequencies") //
    ("mtaniso.sensorReadings", po::value<std::string>(), "MT sensor readings") //
    ("mtaniso.noiseAlpha", po::value<double>(), "noise NIG alpha variable") //
    ("mtaniso.noiseBeta", po::value<double>(), "noise NIG beta variable") //
    ("mtaniso.ignoreAniso", po::value<bool>(), "ignore anisotropy");
  }

  template<>
  MtAnisoSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    MtAnisoSpec spec;
    if (sensorsEnabled.count(ForwardModel::MTANISO))
    {
      std::vector<std::vector<std::string>> data = io::csv::readRaw(vm["mtaniso.sensorLocations"].as<std::string>());
      spec.locations = readFixed<double, 3, 0>(data);
      spec.freqs = readRagged<double, 3>(data);
      spec.noise.inverseGammaAlpha = vm["mtaniso.noiseAlpha"].as<double>();
      spec.noise.inverseGammaBeta = vm["mtaniso.noiseBeta"].as<double>();
      spec.ignoreAniso = vm["mtaniso.ignoreAniso"].as<bool>();
    }
    return spec;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, MtAnisoSpec spec, const po::options_description & od)
  {
    std::vector<std::vector<std::string>> data;
    writeFixed<double>(data, spec.locations);
    writeRagged<double>(data, spec.freqs);
    io::csv::writeRaw(prefix + "sensorInputs.csv", data);

    return build_vm(po::variables_map(), od, "mtaniso",
                    { { "sensorLocations", prefix + "sensorInputs.csv" },
                      { "noiseAlpha", io::to_string(spec.noise.inverseGammaAlpha) },
                      { "noiseBeta", io::to_string(spec.noise.inverseGammaBeta) },
                      { "ignoreAniso", io::to_string(spec.ignoreAniso) } });
  }

  //! @note the sensor params don't actually have anything in them at the moment so we don't need to do any parsing
  //!
  template<>
  MtAnisoParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return MtAnisoParams();
  }

  template<>
  MtAnisoResults parseSensorReadings(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    MtAnisoResults s;
    if (sensorsEnabled.count(ForwardModel::MTANISO))
    {
      std::vector<std::vector<std::string>> data = io::csv::readRaw(vm["mtaniso.sensorReadings"].as<std::string>());
      for (std::vector<std::string> & row : data)
      {
        Eigen::MatrixX4cd reading;
        reading.resize(row.size() / 8, 4);
        for (uint i = 0; i < row.size(); i += 8)
        {
          reading(i / 8, 0) = std::complex<double>(io::from_string<double>(row[i + 0]), io::from_string<double>(row[i + 1]));
          reading(i / 8, 1) = std::complex<double>(io::from_string<double>(row[i + 2]), io::from_string<double>(row[i + 3]));
          reading(i / 8, 2) = std::complex<double>(io::from_string<double>(row[i + 4]), io::from_string<double>(row[i + 5]));
          reading(i / 8, 3) = std::complex<double>(io::from_string<double>(row[i + 6]), io::from_string<double>(row[i + 7]));
        }
        s.readings.push_back(reading);
        s.phaseTensor.push_back(fwd::phaseTensor1d(reading));
        s.alpha.push_back(fwd::alpha1d(s.phaseTensor.back()));
        s.beta.push_back(fwd::beta1d(s.phaseTensor.back()));

      }
    }
    s.likelihood = 0.0;
    return s;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, MtAnisoResults results, const po::options_description & od)
  {
    std::vector<std::vector<std::string>> data;

    for (Eigen::MatrixX4cd & reading : results.readings)
    {
      std::vector<std::string> row;
      for (uint i = 0; i < reading.rows(); i++)
      {
        for (uint j = 0; j < 4; j++)
        {
          row.push_back(io::to_string(std::real(reading(i, j))));
          row.push_back(io::to_string(std::imag(reading(i, j))));
        }
      }
      data.push_back(row);
    }
    io::csv::writeRaw(prefix + "sensorReadings.csv", data);

    return build_vm(po::variables_map(), od, "mtaniso", { { "sensorReadings", prefix + "sensorReadings.csv" } });;
  }

  template<>
  void enableProperties<ForwardModel::MTANISO>(Eigen::VectorXi & propMasksMask)
  {
    propMasksMask(static_cast<uint>(RockProperty::LogResistivityX)) = 1;
    propMasksMask(static_cast<uint>(RockProperty::LogResistivityY)) = 1;
    propMasksMask(static_cast<uint>(RockProperty::LogResistivityZ)) = 1;
    propMasksMask(static_cast<uint>(RockProperty::ResistivityPhase)) = 1;
  }

  template<>
  prior::MtAnisoParamsPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    return prior::MtAnisoParamsPrior();
  }

  template<>
  bool validateSensor(const WorldSpec & world, const MtAnisoSpec & spec, const MtAnisoResults & result)
  {
    bool valid = true;
    if (spec.locations.rows() == 0)
    {
      LOG(ERROR)<< "input: no mt aniso locations specified. Disable forward model if it is not used.";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != spec.freqs.size())
    {
      LOG(ERROR)<< "input: different number of mt aniso freq (" << spec.freqs.size() << ") to location specified. Remove or add more locations ("<< spec.freqs.size() <<").";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.cols()) != 3)
    {
      LOG(ERROR)<< "input: locations in mt aniso must have three cols (x, y, z).";
      valid = false;
    }
    for (uint l = 0; l < spec.locations.rows(); l++)
    {
      if (spec.locations(l, 0) < world.xBounds.first || spec.locations(l, 0) > world.xBounds.second
          || spec.locations(l, 1) < world.yBounds.first || spec.locations(l, 1) > world.yBounds.second)
      {
        LOG(ERROR)<< "input: mt aniso location " << (l + 1) << " is out of world bounds";
        valid = false;
      }
    }
    for (uint l = 0; l < spec.freqs.size(); l++)
    {
      if (spec.freqs[l].rows() == 0)
      {
        LOG(ERROR)<< "input: mt aniso boundary " << (l + 1) << " does not have any frequencies. Remove sensor or add an freqency.";
        valid = false;
      }
      for (uint i = 0; i < spec.freqs[l].rows(); i++)
      {
        if (spec.freqs[l][i] <= 0)
        {
          LOG(ERROR)<< "input: mt aniso boundary " << (l + 1) << " for layer " << (l + 1) << " must be greather than 0";
          valid = false;
        }
      }
    }
    if (spec.noise.inverseGammaAlpha <= 0 || spec.noise.inverseGammaAlpha <= 0)
    {
      LOG(ERROR)<< "input: mt aniso noise parameters must be greater than 0";
      valid = false;
    }
    if (static_cast<uint>(spec.locations.rows()) != result.readings.size())
    {
      LOG(ERROR)<< "input: different number of readings for mt aniso results (" << result.readings.size() << ") to location specified. Remove or add more locations ("<< spec.locations.rows() <<").";
      valid = false;
    }
    for (uint l = 0; l < result.readings.size(); l++)
    {
      if (result.readings[l].rows() == 0)
      {
        LOG(ERROR)<< "input: mt aniso locations " << (l + 1) << " readings are empty.";
        valid = false;
      }
      if (static_cast<uint>(result.readings[l].rows()) != spec.freqs[l].size())
      {
        LOG(ERROR)<< "input: mt aniso location " << (l + 1) << " readings should match the number of frequencies: " << result.readings[l].rows() << " vs " << spec.freqs[l].size();
        valid = false;
      }
    }
    return valid;
  }
}
