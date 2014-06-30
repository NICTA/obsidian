//!
//! \file serial/world.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "serial/world.hpp"
#include "serial/serialtypes.pb.h" // should have been generated in current dir
#include "serial/utility.hpp"

namespace obsidian
{
  namespace comms
  {

    std::string serialise(const WorldSpec& g)
    {
      WorldSpecProtobuf pb;
      pb.set_xmin(g.xBounds.first);
      pb.set_xmax(g.xBounds.second);
      pb.set_ymin(g.yBounds.first);
      pb.set_ymax(g.yBounds.second);
      pb.set_zmin(g.zBounds.first);
      pb.set_zmax(g.zBounds.second);
      pb.set_boundariesaretimes(g.boundariesAreTimes);
      for (auto& i : g.boundaries)
      {
        BoundarySpecProtobuf* lpb = pb.add_boundaries();
        lpb->set_resx(i.ctrlPointResolution.first);
        lpb->set_resy(i.ctrlPointResolution.second);
        lpb->set_boundaryclass((uint) i.boundaryClass);
        lpb->set_offset(matrixString(i.offset));
        lpb->set_offsetrows(i.offset.rows());
      }
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, WorldSpec& g)
    {
      WorldSpecProtobuf pb;
      pb.ParseFromString(s);
      g.xBounds = std::make_pair(pb.xmin(), pb.xmax());
      g.yBounds = std::make_pair(pb.ymin(), pb.ymax());
      g.zBounds = std::make_pair(pb.zmin(), pb.zmax());
      g.boundariesAreTimes = pb.boundariesaretimes();
      for (int i = 0; i < pb.boundaries_size(); i++)
      {
        BoundarySpec l;
        l.ctrlPointResolution = std::make_pair(pb.boundaries(i).resx(), pb.boundaries(i).resy());
        l.boundaryClass = (BoundaryClass) pb.boundaries(i).boundaryclass();
        uint rows = pb.boundaries(i).offsetrows();
        l.offset = stringMatrix(pb.boundaries(i).offset(), rows);
        g.boundaries.push_back(l);
      }
    }

    std::string serialise(const WorldParams& g)
    {
      WorldParamsProtobuf pb;
      for (uint i = 0; i < g.rockProperties.size(); i++)
      {
        pb.add_rockproperties(vectorString(g.rockProperties[i]));
      }
      for (uint i = 0; i < g.controlPoints.size(); i++)
      {
        pb.add_controlpoints(matrixString(g.controlPoints[i]));
        pb.add_controlpointrows(g.controlPoints[i].rows());
      }
      return protobufToString(pb);
    }

    void unserialise(const std::string& s, WorldParams& g)
    {
      WorldParamsProtobuf pb;
      pb.ParseFromString(s);
      for (int i = 0; i < pb.rockproperties_size(); i++)
      {
        g.rockProperties.push_back(stringVector(pb.rockproperties(i)));
      }
      for (int i = 0; i < pb.controlpoints_size(); i++)
      {
        uint rows = pb.controlpointrows(i);
        g.controlPoints.push_back(stringMatrix(pb.controlpoints(i), rows));
      }
    }

  }
}
