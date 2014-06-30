//!
//! Input implementations related to world model.
//!
//! \file world.cpp
//! \author Lachlan McCalman
//! \author Nahid Akbar
//! \date 2014
//! \license General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "common.hpp"

namespace obsidian
{

  void initWorldOptions(po::options_description & options)
  {
    options.add_options() //
    ("world.xRange", po::value<Eigen::Vector2d>(), "x range in metres") //
    ("world.yRange", po::value<Eigen::Vector2d>(), "y range in metres") //
    ("world.depthRange", po::value<Eigen::Vector2d>(), "depth range") //
    ("boundaries.offsets", po::value<std::string>(), "layer const offset surfaces") //
    ("boundaries.times", po::value<std::string>(), "layer seismic time offset surfaces (see useTimes)") //
    ("boundaries.useTimes", po::value<bool>(), "boundaries are times; use boundaryTimes") //
    ("boundaries.types", po::value<std::string>(), "boundary types (eg normal, warped)") //
    ("boundaries.uncoupledSDs", po::value<Eigen::VectorXd>(), "layer uncoupled SDs") //
    ("boundaries.coupledSDs", po::value<Eigen::VectorXd>(), "layer coupled SDs") //
    ("simulation.ctrlPoints", po::value<std::string>(), "true control points") //
    ("boundaries.ctrlPointMasks", po::value<std::string>(), "layer stochastic masks") //
    ("boundaries.ctrlPointMins", po::value<std::string>(), "layer control point absolute minimum values") //
    ("boundaries.ctrlPointMaxs", po::value<std::string>(), "layer control point absolute maximum values") //
    ("simulation.layerProperties", po::value<std::string>(), "true rock properties") //
    ("rocks.masks", po::value<std::string>(), "rock property masks") //
    ("rocks.means", po::value<std::string>(), "layer property means") //
    ("rocks.mins", po::value<std::string>(), "layer absolute minimum value") //
    ("rocks.maxs", po::value<std::string>(), "layer absolute maximum value") //
    ("rocks.covariances", po::value<std::string>(), "layer property covariances") //
    ("initialisation.ctrlPoints", po::value<std::string>(), "stack lowest chain control points") //
    ("initialisation.layerProperties", po::value<std::string>(), "stack lowest chain rock properties");
  }

  const std::map<std::string, BoundaryClass> boundaryClassMap { { "normal", BoundaryClass::Normal }, { "warped", BoundaryClass::Warped } };
  const std::map<BoundaryClass, std::string> boundaryClassStrMap { { BoundaryClass::Normal, "normal", }, { BoundaryClass::Warped, "warped" } };

  template<typename T> Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> flip(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> & input)
  {
    // Make sure these are the right shape from the csv input
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> output = input.transpose();
//    return output.rowwise().reverse();
    return output.rowwise().reverse();
  }

  template<typename T> Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> unflip(
      const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> & input)
  {
    // undo effects of flip before writing
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> output = input.rowwise().reverse();
    return output.transpose();
  }

  template<>
  WorldSpec parseSpec(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    // World model
    Eigen::Vector2d xRange = vm["world.xRange"].as<Eigen::Vector2d>();
    Eigen::Vector2d yRange = vm["world.yRange"].as<Eigen::Vector2d>();
    Eigen::Vector2d depthRange = vm["world.depthRange"].as<Eigen::Vector2d>();

    WorldSpec region(xRange(0), xRange(1), yRange(0), yRange(1), depthRange(0), depthRange(1));
    bool boundariesAreTimes = vm["boundaries.useTimes"].as<bool>();
    region.boundariesAreTimes = boundariesAreTimes;
    auto boundaryOffsetsFiles = io::split(vm["boundaries.offsets"].as<std::string>());
    auto boundaryTimeFiles = io::split(vm["boundaries.times"].as<std::string>());
    auto ctrlPointMasks = io::split(vm["boundaries.ctrlPointMasks"].as<std::string>());
    auto boundaryTypes = io::split(vm["boundaries.types"].as<std::string>());
    uint nBoundaries;
    if (boundariesAreTimes)
      nBoundaries = boundaryTimeFiles.size();
    else
      nBoundaries = boundaryOffsetsFiles.size();
    for (uint i = 0; i < nBoundaries; i++)
    {
      Eigen::MatrixXi ctrlPointMask = flip(io::csv::read<int>(ctrlPointMasks[i]));
      Eigen::MatrixXd bOffset;
      if (boundariesAreTimes)
        bOffset = (io::csv::read<double>(boundaryTimeFiles[i]));
      else
        bOffset = (io::csv::read<double>(boundaryOffsetsFiles[i]));
      uint nX = ctrlPointMask.rows();
      uint nY = ctrlPointMask.cols();
      BoundarySpec bspec;
      bspec.boundaryClass = boundaryClassMap.at(boundaryTypes[i]);
      bspec.offset = flip(bOffset);
      bspec.ctrlPointResolution = std::make_pair(nX, nY);
      region.boundaries.push_back(bspec);
    }
    return region;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, WorldSpec spec, const po::options_description & od)
  {
    std::vector<std::string> times;
    std::vector<std::string> offsets;
    std::vector<std::string> types;
    std::vector<std::string> ctrlPointMasks;

    for (uint i = 0; i < spec.boundaries.size(); i++)
    {
      BoundarySpec & bspec = spec.boundaries[i];
      if (spec.boundariesAreTimes)
      {
        std::string fn = prefix + "times" + std::to_string(i + 1) + ".csv";
        io::csv::write(fn, unflip(bspec.offset));
        times.push_back(fn);
      } else
      {
        std::string fn = prefix + "offsets" + std::to_string(i + 1) + ".csv";
        io::csv::write(fn, unflip(bspec.offset));
        offsets.push_back(fn);
      }
      std::string fn = prefix + "controlPointMask" + std::to_string(i + 1) + ".csv";
      io::csv::write(fn, unflip(Eigen::MatrixXi(bspec.ctrlPointResolution.first, bspec.ctrlPointResolution.second).setOnes()));
      ctrlPointMasks.push_back(fn);
      types.push_back(boundaryClassStrMap.at(bspec.boundaryClass));
    }

    return build_vm(
        build_vm(po::variables_map(), od, "world", { { "xRange", io::to_string(spec.xBounds.first, spec.xBounds.second) }, //
                                                     { "yRange", io::to_string(spec.yBounds.first, spec.yBounds.second) },
                                                     { "depthRange", io::to_string(spec.zBounds.first, spec.zBounds.second) } }),
        od, "boundaries",
        { { "useTimes", io::to_string(spec.boundariesAreTimes) },
          { "offsets", io::join(offsets, " ") },
          { "times", io::join(times, " ") },
          { "types", io::join(types, " ") },
          { "ctrlPointMasks", io::join(ctrlPointMasks, " ") } });
  }

  template<>
  WorldParams parseSimulationParams(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    WorldParams params;
    auto trueCrtlPointFiles = io::split(vm["simulation.ctrlPoints"].as<std::string>());
    auto trueLayerFiles = io::split(vm["simulation.layerProperties"].as<std::string>());
    uint n = trueCrtlPointFiles.size();
    CHECK(n == trueLayerFiles.size());
    for (uint i = 0; i < n; i++)
    {
      Eigen::MatrixXd trueCtrlPoints = flip(io::csv::read<double>(trueCrtlPointFiles[i]));
      Eigen::VectorXd props = io::csv::read<double>(trueLayerFiles[i]);

      params.controlPoints.push_back(trueCtrlPoints);
      params.rockProperties.push_back(props);
    }
    return params;
  }

  template<>
  po::variables_map write<>(const std::string & prefix, WorldParams g, const po::options_description & od)
  {
//    io::csv::write<double,Eigen::Dynamic, 1>(prefix + "sensorReadings.csv", g.readings);
    std::vector<std::string> controlPoints;
    for (uint i = 0; i < g.controlPoints.size(); i++)
    {
      std::string fn = prefix + "controlPoints" + std::to_string(i + 1) + ".csv";
      io::csv::write<double>(fn, unflip(g.controlPoints[i]));
      controlPoints.push_back(fn);
    }
    std::vector<std::string> rockProperties;
    for (uint i = 0; i < g.rockProperties.size(); i++)
    {
      std::string fn = prefix + "rockProperties" + std::to_string(i + 1) + ".csv";
      io::csv::write<double>(fn, g.rockProperties[i]);
      rockProperties.push_back(fn);
    }
    return build_vm(po::variables_map(), od, "simulation",
                    { { "ctrlPoints", io::join(controlPoints) }, { "layerProperties", io::join(rockProperties) } });
  }

  template<ForwardModel f>
  struct getSensorPropertiesMask
  {
    getSensorPropertiesMask(Eigen::VectorXi & propMasksMask)
    {
      enableProperties<f>(propMasksMask);
    }
  };

  template<>
  prior::WorldParamsPrior parsePrior(const po::variables_map& vm, const std::set<ForwardModel> & sensorsEnabled)
  {
    std::vector<distrib::MultiGaussian> ctrlPointPrior;
    std::vector<Eigen::MatrixXi> ctrlPointMasks;
    std::vector<Eigen::MatrixXd> ctrlPointMins;
    std::vector<Eigen::MatrixXd> ctrlPointMaxs;

    auto boundaries_ctrlPointMasks = io::split(vm["boundaries.ctrlPointMasks"].as<std::string>());
    auto boundaries_ctrlPointMins = io::split(vm["boundaries.ctrlPointMins"].as<std::string>());
    auto boundaries_ctrlPointMaxs = io::split(vm["boundaries.ctrlPointMaxs"].as<std::string>());
    Eigen::VectorXd uncoupledSDs = vm["boundaries.uncoupledSDs"].as<Eigen::VectorXd>();
    Eigen::VectorXd coupledSDs = vm["boundaries.coupledSDs"].as<Eigen::VectorXd>();

    CHECK(boundaries_ctrlPointMasks.size() == boundaries_ctrlPointMins.size());
    CHECK(boundaries_ctrlPointMasks.size() == boundaries_ctrlPointMaxs.size());
    CHECK(boundaries_ctrlPointMasks.size() == (uint )coupledSDs.size());
    CHECK(boundaries_ctrlPointMasks.size() == (uint )uncoupledSDs.size());

    for (uint i = 0; i < boundaries_ctrlPointMasks.size(); i++)
    {
      Eigen::MatrixXi ctrlPointMask = flip(io::csv::read<int>(boundaries_ctrlPointMasks[i]));
      Eigen::MatrixXd ctrlPointMin = flip(io::csv::read<double>(boundaries_ctrlPointMins[i]));
      Eigen::MatrixXd ctrlPointMax = flip(io::csv::read<double>(boundaries_ctrlPointMaxs[i]));
      const Eigen::MatrixXd zero = Eigen::MatrixXd::Zero(ctrlPointMask.rows(), ctrlPointMask.cols());
      CHECK((ctrlPointMax.array() > ctrlPointMin.array()).all()) << "layer " << i;
      CHECK((zero.array() > ctrlPointMin.array()).all()) << "layer " << i;
      CHECK((ctrlPointMax.array() > zero.array()).all()) << "layer " << i;
      CHECK(coupledSDs(i) != 0.0 or uncoupledSDs(i) != 0) << "layer " << i;
      ctrlPointMasks.push_back(ctrlPointMask);
      ctrlPointMins.push_back(ctrlPointMin);
      ctrlPointMaxs.push_back(ctrlPointMax);
      ctrlPointPrior.push_back(distrib::coupledGaussianBlock(zero, coupledSDs(i), uncoupledSDs(i)));
    }

    // Boundary classes
    std::vector<BoundaryClass> classes;
    std::vector<std::string> boundaryTypes = io::split(vm["boundaries.types"].as<std::string>());
    for (auto const& s : boundaryTypes)
      classes.push_back(boundaryClassMap.at(s));

    // Rock property prior
    std::vector<distrib::MultiGaussian> propPrior;
    std::vector<Eigen::VectorXi> propMasks;
    std::vector<Eigen::VectorXd> propMins;
    std::vector<Eigen::VectorXd> propMaxs;

    auto rocks_means = io::split(vm["rocks.means"].as<std::string>());
    auto rocks_mins = io::split(vm["rocks.mins"].as<std::string>());
    auto rocks_maxs = io::split(vm["rocks.maxs"].as<std::string>());
    auto rocks_covariances = io::split(vm["rocks.covariances"].as<std::string>());
    auto rocks_masks = io::split(vm["rocks.masks"].as<std::string>());

    CHECK(rocks_means.size() == rocks_mins.size());
    CHECK(rocks_means.size() == rocks_maxs.size());
    CHECK(rocks_means.size() == rocks_covariances.size());
    CHECK(rocks_means.size() == rocks_masks.size());

    for (uint i = 0; i < rocks_means.size(); i++)
    {
      Eigen::VectorXd rocks_mean = io::csv::read<double, Eigen::Dynamic, 1>(rocks_means[i]);
      Eigen::VectorXd rocks_min = io::csv::read<double, Eigen::Dynamic, 1>(rocks_mins[i]);
      Eigen::VectorXd rocks_max = io::csv::read<double, Eigen::Dynamic, 1>(rocks_maxs[i]);
      Eigen::MatrixXd rocks_covariance = io::csv::read<double>(rocks_covariances[i]);
      Eigen::VectorXi rocks_mask = io::csv::read<int, Eigen::Dynamic, 1>(rocks_masks[i]);
      CHECK(rocks_mean.size() == (uint ) RockProperty::Count) << "layer " << i;
      CHECK(rocks_min.size() == (uint ) RockProperty::Count) << "layer " << i;
      CHECK(rocks_max.size() == (uint ) RockProperty::Count) << "layer " << i;
      CHECK((rocks_max.array() > rocks_min.array()).all()) << "layer " << i;
      CHECK((rocks_mean.array() > rocks_min.array()).all()) << "layer " << i;
      CHECK((rocks_max.array() > rocks_mean.array()).all()) << "layer " << i;
      CHECK(rocks_covariance.rows() == (uint ) RockProperty::Count) << "layer " << i;
      CHECK(rocks_covariance.cols() == (uint ) RockProperty::Count) << "layer " << i;
      CHECK(rocks_mask.size() == (uint ) RockProperty::Count) << "layer " << i;
      if (!(rocks_max.array() > rocks_mean.array() + 1 * rocks_covariance.diagonal().array().sqrt()).all())
      {
        LOG(WARNING)<< "WARNING: layer " << i << " rocks_mean is within 1 sigmas of rocks_max (" << (rocks_max.array() > rocks_mean.array() + 1 * rocks_covariance.diagonal().array().sqrt()).transpose() << ")";
      }
      if (!(rocks_min.array() < rocks_mean.array() - 1 * rocks_covariance.diagonal().array().sqrt()).all())
      {
        LOG(WARNING)<< "WARNING: layer " << i << " rocks_mean is within 1 sigmas of rocks_min (" << (rocks_min.array() < rocks_mean.array() - 1 * rocks_covariance.diagonal().array().sqrt()).transpose() << ")";
      }
      propMasks.push_back(rocks_mask);
      propMins.push_back(rocks_min);
      propMaxs.push_back(rocks_max);
      propPrior.push_back(distrib::MultiGaussian(rocks_mean, rocks_covariance));
    }

    Eigen::VectorXi propMasksMask = Eigen::VectorXi::Zero(static_cast<uint>(RockProperty::Count));
    applyToSensorsEnabled<getSensorPropertiesMask>(sensorsEnabled, std::ref(propMasksMask));

    for (Eigen::VectorXi & propMask : propMasks)
    {
      if (vm["boundaries.useTimes"].as<bool>())
        propMask(static_cast<uint>(RockProperty::PWaveVelocity)) = 1;
      if (vm["mtaniso.ignoreAniso"].as<bool>())
      {
        propMask(static_cast<uint>(RockProperty::LogResistivityY)) = 0;
        propMask(static_cast<uint>(RockProperty::LogResistivityZ)) = 0;
        propMask(static_cast<uint>(RockProperty::ResistivityPhase)) = 0;
      }
      for (uint p = 0; p < static_cast<uint>(RockProperty::Count); p++)
      {
        if (!propMasksMask(p))
          propMask(p) = 0;
      }
    }

    // TODO: if there are other stochastic params, parse them here
    // prior doesnt have a trivial constructor
    return prior::WorldParamsPrior(ctrlPointPrior, ctrlPointMasks, ctrlPointMins, ctrlPointMaxs, coupledSDs, uncoupledSDs, propPrior,
                                   propMasks, propMins, propMaxs, classes);
  }

  template<ForwardModel f>
  struct setSensorsEnabled
  {
    setSensorsEnabled(po::variables_map & vm)
    {
      po::options_description od;
      initSensorInputFileOptions<f>(od);
      vm = build_vm(vm, od, configHeading<f>(), { { "enabled", io::to_string(true) } });
    }
  };

  template<>
  po::variables_map write<>(const std::string & prefix, prior::WorldParamsPrior g, const po::options_description & od)
  {
    std::vector<std::string> boundaries_ctrlPointMasks;
    std::vector<std::string> boundaries_ctrlPointMins;
    std::vector<std::string> boundaries_ctrlPointMaxs;
    std::vector<std::string> boundaries_coupledSDs;
    std::vector<std::string> boundaries_uncoupledSDs;
    std::vector<std::string> boundaries_types;

    std::vector<std::string> rocks_means;
    std::vector<std::string> rocks_mins;
    std::vector<std::string> rocks_maxs;
    std::vector<std::string> rocks_covariances;
    std::vector<std::string> rocks_masks;

    for (uint l = 0; l < g.propMasks.size(); l++)
    {

      boundaries_types.push_back(boundaryClassStrMap.at(g.classes[l]));

      std::string fn;

      fn = prefix + "layer" + std::to_string(l + 1) + "ctrlPointMasks.csv";
      io::csv::write<int>(fn, unflip(g.ctrlptMasks[l]));
      boundaries_ctrlPointMasks.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "ctrlPointMins.csv";
      io::csv::write<double>(fn, unflip(g.ctrlptMins[l]));
      boundaries_ctrlPointMins.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "ctrlPointMaxs.csv";
      io::csv::write<double>(fn, unflip(g.ctrlptMaxs[l]));
      boundaries_ctrlPointMaxs.push_back(fn);

      boundaries_coupledSDs.push_back(io::to_string(g.ctrlptCoupledSds(l)));

      boundaries_uncoupledSDs.push_back(io::to_string(g.ctrlptUncoupledSds(l)));

      fn = prefix + "layer" + std::to_string(l + 1) + "rocksMeans.csv";
      io::csv::write<double>(fn, g.propertyPrior[l].mu);
      rocks_means.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "rocksMins.csv";
      io::csv::write<double>(fn, g.propMins[l]);
      rocks_mins.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "rocksMaxs.csv";
      io::csv::write<double>(fn, g.propMaxs[l]);
      rocks_maxs.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "rocksCovariance.csv";
      io::csv::write<double>(fn, g.propertyPrior[l].sigma);
      rocks_covariances.push_back(fn);

      fn = prefix + "layer" + std::to_string(l + 1) + "rocksMask.csv";
      io::csv::write<int>(fn, g.propMasks[l]);
      rocks_masks.push_back(fn);
    }

    po::variables_map vm;
    applyToSensors<setSensorsEnabled>(std::ref(vm));

    return build_vm(
        build_vm(
            build_vm(vm, od, "boundaries",
                     { { "ctrlPointMasks", io::join(boundaries_ctrlPointMasks) },
                       { "ctrlPointMins", io::join(boundaries_ctrlPointMins) },
                       { "ctrlPointMaxs", io::join(boundaries_ctrlPointMaxs) },
                       { "coupledSDs", io::join(boundaries_coupledSDs) },
                       { "uncoupledSDs", io::join(boundaries_uncoupledSDs) },
                       { "useTimes", io::to_string(false) },
                       { "types", io::join(boundaries_types) } }),
            od, "rocks", { { "means", io::join(rocks_means) },
                           { "mins", io::join(rocks_mins) },
                           { "maxs", io::join(rocks_maxs) },
                           { "covariances", io::join(rocks_covariances) },
                           { "masks", io::join(rocks_masks) } }),
        od, "mtaniso", { { "ignoreAniso", io::to_string(false) } });
  }

  template<>
  std::vector<WorldParams> parseInitStates(const po::variables_map& vm, const WorldSpec & spec,
                                           const std::set<ForwardModel> & sensorsEnabled)
  {
    LOG(INFO)<< "Loading initial states";
    // reuse parseSimulationParams<WorldParams>
    po::options_description od;
    initWorldOptions(od);

    std::vector<std::string> controlPoints = io::split(vm["initialisation.ctrlPoints"].as<std::string>());
    std::vector<std::string> layerProperties = io::split(vm["initialisation.layerProperties"].as<std::string>());
    std::vector<WorldParams> params;
    uint count = std::min(controlPoints.size(), layerProperties.size()) / spec.boundaries.size();
    if (count * spec.boundaries.size() != controlPoints.size())
    {
      LOG (ERROR) << "input: Invalid number of ctrlPoints specified in initialisation";
    }
    if (count * spec.boundaries.size() != layerProperties.size())
    {
      LOG (ERROR) << "input: Invalid number of layerProperties specified in initialisation";
    }
    LOG(INFO)<< count << "initial states";
    for (uint s = 0; s < count; s++)
    {
      std::vector<std::string> stackControlPoints;
      std::vector<std::string> stackLayerProperties;
      for (uint l = 0; l < spec.boundaries.size(); l++)
      {
        stackControlPoints.push_back(controlPoints[s * spec.boundaries.size() + l]);
        stackLayerProperties.push_back(layerProperties[s * spec.boundaries.size() + l]);
      }

      po::variables_map vm = build_vm(po::variables_map(), od, "simulation",
          {
            { "ctrlPoints", io::join(stackControlPoints)},
            { "layerProperties", io::join(stackLayerProperties)}});
      params.push_back(parseSimulationParams<WorldParams>(vm, sensorsEnabled));
    }
    LOG(INFO) << "Loaded " << params.size() << " initial states";
    return params;
  }

  bool validateWorld(const WorldSpec & spec, const WorldParams & param)
  {
    LOG(INFO)<<"input: Validating world";
  bool valid = true;
  if (spec.xBounds.second <= spec.xBounds.first)
  {
    LOG(ERROR)<< "input: xBounds max (" << spec.xBounds.second << ") should be more than min(" << spec.xBounds.first <<")";
    valid = false;
  }
  if (spec.yBounds.second <= spec.yBounds.first)
  {
    LOG(ERROR)<< "input: yBounds max (" << spec.yBounds.second << ") should be more than min(" << spec.yBounds.first <<")";
    valid = false;
  }
  if (spec.zBounds.second <= spec.zBounds.first)
  {
    LOG(ERROR)<< "input: zBounds max (" << spec.zBounds.second << ") should be more than min(" << spec.zBounds.first <<")";
    valid = false;
  }
  if (spec.boundaries.size() == 0)
  {
    LOG(ERROR)<< "input: world needs at least one boundary, has " << spec.boundaries.size();
    valid = false;
  }
  for (uint b = 0; b < spec.boundaries.size(); b++)
  {
    const BoundarySpec & bspec = spec.boundaries[b];
    if (bspec.offset.rows() == 0 || bspec.offset.cols() == 0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " offset is invalid. Need at least 1x1 (flat), found " << bspec.offset.rows() << "x" << bspec.offset.cols();
      valid = false;
    }
    for (uint i = 0; i < bspec.offset.rows(); i++)
    {
      for (uint j = 0; j < bspec.offset.cols(); j++)
      {
        if (bspec.offset(i, j) < spec.zBounds.first)
        {
          LOG(ERROR)<< "input: boundary " << (b + 1) << " offset point " << (i + 1) << ", " << (j + 1) << " is less than depthRange min";
          valid = false;
        }
        if (bspec.offset(i, j) > spec.zBounds.second)
        {
          LOG(ERROR)<< "input: boundary " << (b + 1) << " offset point " << (i + 1) << ", " << (j + 1) << " is greater than depthRange max";
          valid = false;
        }
        if (bspec.ctrlPointResolution.first == 0 || bspec.ctrlPointResolution.second == 0)
        {
          LOG(ERROR)<< "input: boundary " << (b + 1) << " control point must be at least 1x1, found" << bspec.ctrlPointResolution.first << "x" << bspec.ctrlPointResolution.second;
          valid = false;
        }
      }
    }
  }
  if (param.rockProperties.size() != spec.boundaries.size())
  {
    LOG(ERROR)<< "input: number of sets of layerProperties (" << param.rockProperties.size() << ") must be equal to number of boundaries (" << spec.boundaries.size() << ") ";
    valid = false;
  }
  if (param.controlPoints.size() != spec.boundaries.size())
  {
    LOG(ERROR)<< "input: number of sets of ctrlPoints (" << param.controlPoints.size() << ") must be equal to number of boundaries (" << spec.boundaries.size() << ") ";
    valid = false;
  }
  for (uint b = 0; b < param.rockProperties.size(); b++)
  {
    const Eigen::VectorXd & rockProperty = param.rockProperties[b];
    if (rockProperty.rows() != static_cast<uint>(RockProperty::Count))
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " layerProperties does not have " << (static_cast<uint>(RockProperty::Count)) << " properties";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::Density)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " Density LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::ThermalConductivity)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " ThermalConductivity LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::ThermalProductivity)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " ThermalProductivity LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::LogResistivityX)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " LogResistivityX LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::LogResistivityY)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " LogResistivityY LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::LogResistivityZ)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " LogResistivityZ LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::ResistivityPhase)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " ResistivityPhase LayerProperties must be positive";
      valid = false;
    }
    if (rockProperty[static_cast<uint>(RockProperty::PWaveVelocity)] < 0.0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " PWaveVelocity LayerProperties must be positive";
      valid = false;
    }
  }
  for (uint b = 0; b < param.controlPoints.size(); b++)
  {
    const Eigen::MatrixXd & ctrlPoints = param.controlPoints[b];
    if (ctrlPoints.rows() == 0 || ctrlPoints.cols() == 0)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " ctrlPoints point must be at least 1x1";
      valid = false;
    }
    if (ctrlPoints.rows() != spec.boundaries[b].ctrlPointResolution.first || ctrlPoints.cols() != spec.boundaries[b].ctrlPointResolution.second)
    {
      LOG(ERROR)<< "input: boundary " << (b + 1) << " ctrlPoints resolution (" << ctrlPoints.rows() << "x" << ctrlPoints.cols() << ") do not match mask (" << spec.boundaries[b].ctrlPointResolution.first << "x" << spec.boundaries[b].ctrlPointResolution.second << ")";
      valid = false;
    }
  }
  return valid;
}

}
