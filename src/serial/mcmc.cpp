//!
//! \file serial/mcmc.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/mcmc.hpp"
#include "serial/stateline.pb.h"

namespace stateline
{
  namespace comms
  {

    std::string serialise(const mcmc::State& s)
    {
      MCMCStateProtobuf pb;
      pb.set_sample(obsidian::comms::vectorString(s.sample));
      pb.set_energy(s.energy);
      pb.set_beta(s.beta);
      pb.set_accepted(s.accepted);
      pb.set_swaptype((uint) s.swapType);
      return obsidian::comms::protobufToString(pb);
    }

    void unserialise(const std::string& s, mcmc::State& g)
    {
      MCMCStateProtobuf pb;
      pb.ParseFromString(s);
      g.sample = obsidian::comms::stringVector(pb.sample());
      g.energy = pb.energy();
      g.beta = pb.beta();
      g.accepted = pb.accepted();
      g.swapType = (mcmc::SwapType) pb.swaptype();
    }
  }
}
