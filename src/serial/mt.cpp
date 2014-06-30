//!
//! \file serial/mt.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/mt.hpp"
#include "serial/serialtypes.pb.h"

namespace obsidian
{
  namespace comms
  {
    std::string serialise(const MtAnisoSpec& g)
    {
      MtAnisoSpecProtobuf pb;
      pb.set_numlocations(g.locations.rows());
      pb.set_locations(matrixString(g.locations));
      for (uint i = 0; i < g.freqs.size(); i++)
      {
        pb.add_numfreqs(g.freqs[i].rows());
        pb.add_freqs(matrixString(g.freqs[i]));
      }
      pb.set_ignoreaniso(g.ignoreAniso);

      NoiseSpecProtobuf* npb = pb.mutable_noise();
      npb->set_inversegammaalpha(g.noise.inverseGammaAlpha);
      npb->set_inversegammabeta(g.noise.inverseGammaBeta);

      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MtAnisoSpec& g)
    {
      MtAnisoSpecProtobuf pb;
      pb.ParseFromString(s);
      g.locations = stringMatrix(pb.locations(), pb.numlocations());
      for (int i = 0; i < pb.numfreqs_size(); i++)
      {
        g.freqs.push_back(stringMatrix<double, D, 1>(pb.freqs(i), pb.numfreqs(i)));
      }
      g.noise.inverseGammaAlpha = pb.noise().inversegammaalpha();
      g.noise.inverseGammaBeta = pb.noise().inversegammabeta();
      g.ignoreAniso = pb.ignoreaniso();
    }

    std::string serialise(const MtAnisoParams& g)
    {
      MtAnisoParamsProtobuf pb;
      pb.set_returnsensordata(g.returnSensorData);
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MtAnisoParams& g)
    {
      MtAnisoParamsProtobuf pb;
      pb.ParseFromString(s);
      g.returnSensorData = pb.returnsensordata();
    }

    std::string serialise(const MtAnisoResults& g)
    {
      MtAnisoResultsProtobuf pb;
      pb.set_likelihood(g.likelihood);

      for (uint i = 0; i < g.readings.size(); i++)
      {
        pb.add_numfreqs(g.readings[i].rows());
        pb.add_readings(cmplxMatrixString(g.readings[i]));
        pb.add_phasetensor(matrixString(g.phaseTensor[i]));
        pb.add_alpha(matrixString(g.alpha[i]));
        pb.add_beta(matrixString(g.beta[i]));
      }

      return protobufToString(pb);
    }

    void unserialise(const std::string& s, MtAnisoResults& g)
    {
      MtAnisoResultsProtobuf pb;
      pb.ParseFromString(s);
      g.likelihood = pb.likelihood();

      for (int i = 0; i < pb.readings_size(); i++)
      {
        uint numFreqs = pb.numfreqs(i);
        g.readings.push_back(stringCmplxMatrix(pb.readings(i), numFreqs));
        g.phaseTensor.push_back(stringMatrix(pb.phasetensor(i), numFreqs));
        g.alpha.push_back(stringMatrix(pb.alpha(i), numFreqs));
        g.beta.push_back(stringMatrix(pb.beta(i), numFreqs));
      }

    }
  }
}
