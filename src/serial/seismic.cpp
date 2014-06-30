//!
//! \file serial/seismic.cpp
//! \author Nahid Akbar
//! \date May, 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/seismic.hpp"
#include "serial/serialtypes.pb.h"
#include "serial/utility.hpp"

namespace obsidian
{
  namespace comms
  {

    std::string serialise(const Seismic1dSpec& g)
    {
      Seismic1dSpecProtobuf pb;
      pb.set_numlocations(g.locations.rows());
      pb.set_locations(matrixString(g.locations));

      for (const Eigen::VectorXi & interfaces : g.interfaces)
      {
        pb.add_numinterfaces(interfaces.rows());
        pb.add_interfaces(matrixString(interfaces));
      }

      NoiseSpecProtobuf* npb = pb.mutable_noise();
      npb->set_inversegammaalpha(g.noise.inverseGammaAlpha);
      npb->set_inversegammabeta(g.noise.inverseGammaBeta);
      return protobufToString(pb);
    }
    void unserialise(const std::string& s, Seismic1dSpec& g)
    {
      Seismic1dSpecProtobuf pb;
      pb.ParseFromString(s);

      g.locations = stringMatrix(pb.locations(), pb.numlocations());

      g.interfaces.resize(pb.interfaces_size());
      int index = -1;
      while (++index < pb.interfaces_size())
      {
        g.interfaces[index] = stringMatrix<int, D, 1>(pb.interfaces(index), pb.numinterfaces(index));
      }

      g.noise.inverseGammaAlpha = pb.noise().inversegammaalpha();
      g.noise.inverseGammaBeta = pb.noise().inversegammabeta();
    }
    std::string serialise(const Seismic1dParams& g)
    {
      Seismic1dParamsProtobuf pb;
      pb.set_returnsensordata(g.returnSensorData);
      return protobufToString(pb);
    }
    void unserialise(const std::string& s, Seismic1dParams& g)
    {
      Seismic1dParamsProtobuf pb;
      pb.ParseFromString(s);
      g.returnSensorData = pb.returnsensordata();
    }
    std::string serialise(const Seismic1dResults& g)
    {
      Seismic1dResultsProtobuf pb;
      pb.set_likelihood(g.likelihood);

      for (const Eigen::VectorXd & reading : g.readings)
      {
        pb.add_numreadings(reading.rows());
        pb.add_readings(matrixString(reading));
      }

      return protobufToString(pb);
    }
    void unserialise(const std::string& s, Seismic1dResults& g)
    {
      Seismic1dResultsProtobuf pb;
      pb.ParseFromString(s);
      g.likelihood = pb.likelihood();
      g.readings.resize(pb.numreadings_size());
      int index = -1;
      while (++index < pb.numreadings_size())
      {
        g.readings[index] = stringMatrix<double, D, 1>(pb.readings(index), pb.numreadings(index));
      }
    }
  }
}
