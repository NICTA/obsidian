//!
//! \file serial/magnetic.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/magnetic.hpp"
#include "serial/serialtypes.pb.h" // should have been generated in current dir
#include "serial/utility.hpp"

namespace obsidian
{
  namespace comms
  {
    std::string serialise(const MagSpec& m)
    {
      MagSpecProtobuf pb;
      pb.set_numlocations(m.locations.rows());
      pb.set_locations(matrixString(m.locations));
      VoxelisationProtobuf* vpb = pb.mutable_voxelisation();
      vpb->set_xresolution(m.voxelisation.xResolution);
      vpb->set_yresolution(m.voxelisation.yResolution);
      vpb->set_zresolution(m.voxelisation.zResolution);
      vpb->set_supersample(m.voxelisation.supersample);
      NoiseSpecProtobuf* npb = pb.mutable_noise();
      npb->set_inversegammaalpha(m.noise.inverseGammaAlpha);
      npb->set_inversegammabeta(m.noise.inverseGammaBeta);
      pb.set_backgroundfield(vectorString(m.backgroundField));
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MagSpec& m)
    {
      MagSpecProtobuf pb;
      pb.ParseFromString(s);
      m.locations = stringMatrix(pb.locations(), pb.numlocations());
      m.voxelisation.xResolution = pb.voxelisation().xresolution();
      m.voxelisation.yResolution = pb.voxelisation().yresolution();
      m.voxelisation.zResolution = pb.voxelisation().zresolution();
      m.voxelisation.supersample = pb.voxelisation().supersample();
      m.noise.inverseGammaAlpha = pb.noise().inversegammaalpha();
      m.noise.inverseGammaBeta = pb.noise().inversegammabeta();
      m.backgroundField = stringVector(pb.backgroundfield());
    }

    std::string serialise(const MagParams& m)
    {
      MagParamsProtobuf pb;
      pb.set_returnsensordata(m.returnSensorData);
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MagParams& m)
    {
      MagParamsProtobuf pb;
      pb.ParseFromString(s);
      m.returnSensorData = pb.returnsensordata();
    }

    std::string serialise(const MagResults& m)
    {
      MagResultsProtobuf pb;
      pb.set_likelihood(m.likelihood);
      if (m.readings.size() > 0)
      {
        pb.set_numreadings(m.readings.size());
        pb.set_readings(matrixString(m.readings));
      }
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MagResults& m)
    {
      MagResultsProtobuf pb;
      pb.ParseFromString(s);
      m.likelihood = pb.likelihood();
      if (pb.has_readings())
      {
        m.readings = stringMatrix(pb.readings(), pb.numreadings());
      }
    }

  } // namespace comms
} // namespace obsidian
