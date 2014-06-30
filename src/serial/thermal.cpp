//!
//! \file serial/thermal.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/thermal.hpp"
#include "serial/serialtypes.pb.h" // should have been generated in current dir
#include "serial/utility.hpp"

namespace obsidian
{
  namespace comms
  {
    std::string serialise(const ThermalSpec& g)
    {
      ThermalSpecProtobuf pb;
      pb.set_numlocations(g.locations.rows());
      pb.set_locations(matrixString(g.locations));
      pb.set_surfacetemperature(g.surfaceTemperature);
      pb.set_lowerboundary(g.lowerBoundary);
      pb.set_lowerboundaryisheatflow(g.lowerBoundaryIsHeatFlow);
      VoxelisationProtobuf* vpb = pb.mutable_voxelisation();
      vpb->set_xresolution(g.voxelisation.xResolution);
      vpb->set_yresolution(g.voxelisation.yResolution);
      vpb->set_zresolution(g.voxelisation.zResolution);
      vpb->set_supersample(g.voxelisation.supersample);
      NoiseSpecProtobuf* npb = pb.mutable_noise();
      npb->set_inversegammaalpha(g.noise.inverseGammaAlpha);
      npb->set_inversegammabeta(g.noise.inverseGammaBeta);
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, ThermalSpec& g)
    {
      ThermalSpecProtobuf pb;
      pb.ParseFromString(s);
      g.locations = stringMatrix(pb.locations(), pb.numlocations());
      g.surfaceTemperature = pb.surfacetemperature();
      g.lowerBoundary = pb.lowerboundary();
      g.lowerBoundaryIsHeatFlow = pb.lowerboundaryisheatflow();
      g.voxelisation.xResolution = pb.voxelisation().xresolution();
      g.voxelisation.yResolution = pb.voxelisation().yresolution();
      g.voxelisation.zResolution = pb.voxelisation().zresolution();
      g.voxelisation.supersample = pb.voxelisation().supersample();
      g.noise.inverseGammaAlpha = pb.noise().inversegammaalpha();
      g.noise.inverseGammaBeta = pb.noise().inversegammabeta();
    }

    std::string serialise(const ThermalParams& g)
    {
      ThermalParamsProtobuf pb;
      pb.set_returnsensordata(g.returnSensorData);
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, ThermalParams& g)
    {
      ThermalParamsProtobuf pb;
      pb.ParseFromString(s);
      g.returnSensorData = pb.returnsensordata();
    }

    std::string serialise(const ThermalResults& g)
    {
      ThermalResultsProtobuf pb;
      pb.set_likelihood(g.likelihood);
      if (g.readings.size() > 0)
      {
        pb.set_numreadings(g.readings.size());
        pb.set_readings(matrixString(g.readings));
      }
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, ThermalResults& g)
    {
      ThermalResultsProtobuf pb;
      pb.ParseFromString(s);
      g.likelihood = pb.likelihood();
      if (pb.has_readings())
      {
        g.readings = stringMatrix(pb.readings(), pb.numreadings());
      }
    }

  } // namespace comms
} // namespace obsidian
