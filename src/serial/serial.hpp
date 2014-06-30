//!
//! Integration of serialisation methods of jobs and results.
//!
//! \file serial/serial.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatype/datatypes.hpp"
// #include "serial/serialtypes.pb.h" // should have been generated in current dir
#include "serial/world.hpp"
#include "serial/gravity.hpp"
#include "serial/magnetic.hpp"
#include "serial/mt.hpp"
#include "serial/seismic.hpp"
#include "serial/contactpoint.hpp"
#include "serial/thermal.hpp"
#include "serial/commstypes.hpp"
#include "comms/datatypes.hpp"

namespace obsidian
{
  namespace comms
  {

    //Jobs
    template<ForwardModel f>
    inline stateline::comms::JobData serialiseJob(const typename Types<f>::Params& params, const std::string& globalData)
    {
      stateline::comms::JobData job;
      job.type = (uint) f;
      job.globalData = globalData;
      job.jobData = serialise(params);
      return job;
    }

    template<ForwardModel f>
    inline void unserialiseJob(const stateline::comms::JobData & data, typename Types<f>::GloabalParams& g, typename Types<f>::Params& j)
    {
      unserialise(data.jobData, j);
      unserialise(data.globalData, g);
    }

    // Results
    template<ForwardModel f>
    inline stateline::comms::ResultData serialiseResult(const typename Types<f>::Results& r)
    {
      stateline::comms::ResultData d;
      d.type = (uint) f;
      d.data = serialise(r);
      return d;
    }

    template<ForwardModel f>
    inline void unserialiseResult(const stateline::comms::ResultData& data, typename Types<f>::Results& r)
    {
      unserialise(data.data, r);
    }

  } // namespace comms
} // namespace obsidian
