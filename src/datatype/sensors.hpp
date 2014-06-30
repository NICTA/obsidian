//!
//! One place integration point for sensors.
//!
//! \file datatype/sensors.hpp
//! \author Nahid Akbar
//! \date May 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include "datatypes.hpp"

namespace obsidian
{
  template<template<ForwardModel f> class X, typename ... P>
  void applyToSensorsEnabled(const std::set<ForwardModel> & enabled, P ... p)
  {
    if (enabled.count(ForwardModel::GRAVITY))
    {
      X<ForwardModel::GRAVITY>(p...);
    }
    if (enabled.count(ForwardModel::MAGNETICS))
    {
      X<ForwardModel::MAGNETICS>(p...);
    }
    if (enabled.count(ForwardModel::MTANISO))
    {
      X<ForwardModel::MTANISO>(p...);
    }
    if (enabled.count(ForwardModel::SEISMIC1D))
    {
      X<ForwardModel::SEISMIC1D>(p...);
    }
    if (enabled.count(ForwardModel::CONTACTPOINT))
    {
      X<ForwardModel::CONTACTPOINT>(p...);
    }
    if (enabled.count(ForwardModel::THERMAL))
    {
      X<ForwardModel::THERMAL>(p...);
    }
  }
  template<template<ForwardModel f> class X, typename ... P>
  void applyToSensors(P ... p)
  {
    std::set<ForwardModel> enabled;
    for (uint f = 0; f < static_cast<uint>(ForwardModel::Count); f++)
    {
      enabled.insert(static_cast<ForwardModel>(f));
    }
    applyToSensorsEnabled<X, P...>(enabled, p...);
  }

  template<ForwardModel f>
  class ModelToString
  {
  public:
    ModelToString(std::ostream& os, ForwardModel g)
    {
      if (f == g)
      {
        os << forwardModelLabel<f>();
      }
    }
  };

  inline std::ostream& operator<<(std::ostream& os, const ForwardModel& f)
  {
    applyToSensors<ModelToString>(std::ref(os), f);
    return os;
  }
}
