//!
//! Shared methods for writing to npz.
//!
//! \file io/dumpnpz.hpp
//! \author Darren Shen
//! \author Nahid Akbar
//! \date June, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/sensors.hpp"
#include "world/transitions.hpp"
#include "world/voxelise.hpp"
#include "io/npy.hpp"

namespace obsidian
{
  void dumpParamsNPZ(io::NpzWriter & writer, std::vector<WorldParams> & params)
  {
    size_t size = params.size();
    std::vector<Eigen::MatrixXd> rockProperties(params[0].rockProperties.size());
    std::vector<Eigen::MatrixXd> ctrlPoints(params[0].rockProperties.size());
    for (uint l = 0; l < params[0].rockProperties.size(); l++)
    {
      rockProperties[l].resize(size, (uint) RockProperty::Count);
      ctrlPoints[l].resize(size, params[0].controlPoints[l].rows() * params[0].controlPoints[l].cols());
    }
    for (uint i = 0; i < size; i++)
    {
      for (uint l = 0; l < params[0].rockProperties.size(); l++)
      {
        rockProperties[l].row(i) = params[i].rockProperties[l];
        ctrlPoints[l].row(i) = Eigen::Map<Eigen::MatrixXd>(params[i].controlPoints[l].data(), 1,
                                                           params[i].controlPoints[l].rows() * params[i].controlPoints[l].cols());
      }
    }
    for (uint l = 0; l < params[0].rockProperties.size(); l++)
    {
      writer.write<double>("layer" + std::to_string(l) + "rockProperties", rockProperties[l]);
      writer.write<double>("layer" + std::to_string(l) + "ctrlPoints", ctrlPoints[l]);
    }
  }

  void dumpPriorNPZ(io::NpzWriter& writer, const std::vector<double>& prior)
  {
    size_t size = prior.size();
    Eigen::VectorXd priorVector(size);
    for (uint i = 0; i < size; i++)
    {
      priorVector(i) = prior[i];
    }
    writer.write<double>("prior", priorVector);
  }

  void dumpEnergyNPZ(io::NpzWriter& writer, const std::vector<double>& energy)
  {
    size_t size = energy.size();
    Eigen::VectorXd energyVector(size);
    for (uint i = 0; i < size; i++)
    {
      energyVector(i) = energy[i];
    }
    writer.write<double>("energy", energyVector);
  }

  void dumpResultsNPZ(io::NpzWriter& writer, std::vector<GlobalResults>& results)
  {
    size_t size = results.size();
    uint nGrav = results[0].grav.readings.size();
    uint nMag = results[0].mag.readings.size();
    uint nMt = results[0].mt.readings.size();
    uint nThermal = results[0].therm.readings.size();

    Eigen::MatrixXd gravReadings(size, nGrav);
    Eigen::MatrixXd magReadings(size, nMag);
    Eigen::MatrixXd thermReadings(size, nThermal);
    std::vector<std::vector<Eigen::VectorXcd>> mtReadings;
    Eigen::MatrixXd nFreqs(size, nMt);
    uint maxFreqs = 0;
    for (uint i = 0; i < size; i++)
    {
      gravReadings.row(i) = results[i].grav.readings;
      magReadings.row(i) = results[i].mag.readings;
      thermReadings.row(i) = results[i].therm.readings;
      std::vector<Eigen::VectorXcd> stateMt;
      for (uint loc = 0; loc < nMt; loc++)
      {
        Eigen::MatrixXcd res = results[i].mt.readings[loc];
        res.resize(Eigen::NoChange, 1);
        stateMt.push_back(res);
        uint freqs = results[i].mt.readings[loc].rows();
        nFreqs(i, loc) = freqs;
        maxFreqs = std::max(freqs, maxFreqs);
      }
      mtReadings.push_back(stateMt);
    }
    Eigen::MatrixXcd mtReadingMatrix = Eigen::MatrixXcd::Zero(size, nMt * maxFreqs * 4);
    for (uint i = 0; i < size; i++)
    {
      uint c = 0;
      for (uint f = 0; f < mtReadings[i].size(); f++)
      {
        // uint s = mtReadings[i][f].size();
        mtReadingMatrix.block(i, c, 1, maxFreqs) = mtReadings[i][f];
        c += maxFreqs;
      }
    }
    writer.write<double>("gravReadings", gravReadings);
    writer.write<double>("magReadings", magReadings);
    writer.write<double>("thermReadings", thermReadings);
    writer.write<std::complex<double>>("mtReadings", mtReadingMatrix);
    writer.write<double>("mtNumFreqs", nFreqs);
  }

  void dumpLikelihoodNPZ(io::NpzWriter& writer, const std::vector<std::vector<double>>& lh)
  {
    Eigen::MatrixXd lhMatrix(lh.size(), lh[0].size());
    for (uint i = 0; i < lh.size(); i++)
    {
      for (uint j = 0; j < lh[0].size(); j++)
      {
        lhMatrix(i, j) = lh[i][j];
      }
    }
    writer.write<double>("likelihoods", lhMatrix);
  }

  void dumpThetaNPZ(io::NpzWriter& writer, const std::vector<Eigen::VectorXd> &thetas)
  {
    Eigen::MatrixXd thetaMatrix(thetas.size(), thetas[0].rows());
    for (uint i = 0; i < thetas.size(); i++)
    {
      for (uint j = 0; j < thetas[0].rows(); j++)
      {
        thetaMatrix(i,j) = thetas[i](j);
      }
    }
    writer.write<double>("thetas", thetaMatrix);
  }

  void dumpPropVoxelsNPZ(io::NpzWriter& writer, const std::vector<Eigen::MatrixXd> &propVoxs)
  {
    std::vector<std::string> rockPropNames
    {
      "Density", "LogSusceptibility", "ThermalConductivity", "ThermalProductivity",
      "LogResistivityX", "LogResistivityY", "LogResistivityZ", "ResistivityPhase", "PWaveVelocity"
    };

    for (uint i = 0; i < propVoxs.size(); i++)
    {
      writer.write<double>(rockPropNames[i], propVoxs[i]);
    }
  }
  
  void dumpLayerVoxelsNPZ(io::NpzWriter& writer, const std::vector<Eigen::MatrixXd> &layerVoxs)
  {
    for (uint i = 0; i < layerVoxs.size(); i++)
    {
      writer.write<double>("layer" + std::to_string(i), layerVoxs[i]);
    }
  }
  
  void dumpTransitionsVoxelsNPZ(io::NpzWriter& writer, const std::vector<Eigen::MatrixXd> &voxs)
  {
    for (uint i = 0; i < voxs.size(); i++)
    {
      writer.write<double>("boundary" + std::to_string(i), voxs[i]);
    }
  }
} // namespace obsidian
