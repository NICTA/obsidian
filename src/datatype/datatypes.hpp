//!
//! Contains global types which is used for using world and all forward model types in bulk.
//!
//! \file datatype/datatypes.hpp
//! \author Lachlan McCalman
//! \date December 2013
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

// TODO see http://stackoverflow.com/questions/12653407/runtime-value-to-type-mapping
// for some good ideas to implement

#include "datatype/world.hpp"
#include "datatype/gravity.hpp"
#include "datatype/magnetic.hpp"
#include "datatype/mt.hpp"
#include "datatype/seismic.hpp"
#include "datatype/contactpoint.hpp"
#include "datatype/thermal.hpp"

namespace obsidian
{
  // specification
  struct GlobalSpec
  {
    WorldSpec world;
    GravSpec grav;
    MagSpec mag;
    MtAnisoSpec mt;
    Seismic1dSpec s1d;
    ContactPointSpec cpoint;
    ThermalSpec therm;
  };

  struct GlobalParams
  {
    WorldParams world;
    GravParams grav;
    MagParams mag;
    MtAnisoParams mt;
    Seismic1dParams s1d;
    ContactPointParams cpoint;
    ThermalParams therm;
  };

  struct GlobalResults
  {
    GravResults grav;
    MagResults mag;
    MtAnisoResults mt;
    Seismic1dResults s1d;
    ContactPointResults cpoint;
    ThermalResults therm;
  };

  struct GlobalCache
  {
    GravCache grav;
    MagCache mag;
    MtAnisoCache mt;
    Seismic1dCache s1d;
    ContactPointCache cpoint;
    ThermalCache therm;
  };

  //! For accessing subfields of global results.
  //!
  template<typename T, typename R> R & globalResult(T&);

  template<> inline GravResults & globalResult<GlobalResults, GravResults>(GlobalResults&g)
  {
    return g.grav;
  }
  template<> inline MagResults & globalResult<GlobalResults, MagResults>(GlobalResults&g)
  {
    return g.mag;
  }
  template<> inline MtAnisoResults & globalResult<GlobalResults, MtAnisoResults>(GlobalResults&g)
  {
    return g.mt;
  }
  template<> inline Seismic1dResults & globalResult<GlobalResults, Seismic1dResults>(GlobalResults&g)
  {
    return g.s1d;
  }
  template<> inline ContactPointResults & globalResult<GlobalResults, ContactPointResults>(GlobalResults&g)
  {
    return g.cpoint;
  }
  template<> inline ThermalResults & globalResult<GlobalResults, ThermalResults>(GlobalResults&g)
  {
    return g.therm;
  }


  //! For accessing subfields of global results.
  //!
  template<typename T, typename R> const R & globalResult(const T&);

  template<> inline const GravResults & globalResult<GlobalResults, GravResults>(const GlobalResults&g)
  {
    return g.grav;
  }
  template<> inline const MagResults & globalResult<GlobalResults, MagResults>(const GlobalResults&g)
  {
    return g.mag;
  }
  template<> inline const MtAnisoResults & globalResult<GlobalResults, MtAnisoResults>(const GlobalResults&g)
  {
    return g.mt;
  }
  template<> inline const Seismic1dResults & globalResult<GlobalResults, Seismic1dResults>(const GlobalResults&g)
  {
    return g.s1d;
  }
  template<> inline const ContactPointResults & globalResult<GlobalResults, ContactPointResults>(const GlobalResults&g)
  {
    return g.cpoint;
  }
  template<> inline const ThermalResults & globalResult<GlobalResults, ThermalResults>(const GlobalResults&g)
  {
    return g.therm;
  }


  //! For accessing subfields of global.
  //!
  template<typename T, typename R> const R & globalSpec(const T&);

  template<> inline const GravSpec & globalSpec<GlobalSpec, GravSpec>(const GlobalSpec&g)
  {
    return g.grav;
  }
  template<> inline const MagSpec & globalSpec<GlobalSpec, MagSpec>(const GlobalSpec&g)
  {
    return g.mag;
  }
  template<> inline const MtAnisoSpec & globalSpec<GlobalSpec, MtAnisoSpec>(const GlobalSpec&g)
  {
    return g.mt;
  }
  template<> inline const Seismic1dSpec & globalSpec<GlobalSpec, Seismic1dSpec>(const GlobalSpec&g)
  {
    return g.s1d;
  }
  template<> inline const ContactPointSpec & globalSpec<GlobalSpec, ContactPointSpec>(const GlobalSpec&g)
  {
    return g.cpoint;
  }
  template<> inline const ThermalSpec & globalSpec<GlobalSpec, ThermalSpec>(const GlobalSpec&g)
  {
    return g.therm;
  }


  //! For accessing subfields of global.
  //!
  template<typename T, typename R> const R & globalCache(const T&);

  template<> inline const GravCache & globalCache<GlobalCache, GravCache>(const GlobalCache&g)
  {
    return g.grav;
  }
  template<> inline const MagCache & globalCache<GlobalCache, MagCache>(const GlobalCache&g)
  {
    return g.mag;
  }
  template<> inline const MtAnisoCache & globalCache<GlobalCache, MtAnisoCache>(const GlobalCache&g)
  {
    return g.mt;
  }
  template<> inline const Seismic1dCache & globalCache<GlobalCache, Seismic1dCache>(const GlobalCache&g)
  {
    return g.s1d;
  }
  template<> inline const ContactPointCache & globalCache<GlobalCache, ContactPointCache>(const GlobalCache&g)
  {
    return g.cpoint;
  }
  template<> inline const ThermalCache & globalCache<GlobalCache, ThermalCache>(const GlobalCache&g)
  {
    return g.therm;
  }

} // namespace obsidian
