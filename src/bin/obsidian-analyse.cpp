//!
//! An application for analysing obsidian output
//!
//! \file obsidian-analyse.cpp
//! \author Dave Cole
//! \date 2015
//! \licence Lesser General Public License version 3 or later
//! \copyright (c) 2015, NICTA
//!

#include "input/input.hpp"
#include "datatype/sensors.hpp"
#include "fwdmodel/global.hpp"
#include "likelihood/likelihood.hpp"
#include "detail.hpp"
#include "serial/serial.hpp"
#include "io/string.hpp"
#include "io/npy.hpp"
#include "io/dumpnpz.hpp"
#include "world/transitions.hpp"
#include "world/property.hpp"
#include "world/voxelise.hpp"

#include <stateline/app/logging.hpp>
#include <stateline/app/signal.hpp>
#include <stateline/app/commandline.hpp>
#include <stateline/infer/datatypes.hpp>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <string>
#include <thread>
#include <queue>

using namespace obsidian;
namespace sl = stateline;
namespace po = boost::program_options;
using stateline::mcmc::State;

po::options_description commandLineOptions()
{
  auto opts = po::options_description("Obsidian worker options");
  opts.add_options()
    ("loglevel,l",  po::value<int>()->default_value(0), "Logging level")
    ("xres,x",      po::value<uint>()->default_value(32), "Resolution of the voxelisation in the x direction")
    ("yres,y",      po::value<uint>()->default_value(24), "Resolution of the voxelisation in the y direction")
    ("zres,z",      po::value<uint>()->default_value(30), "Resolution of the voxelisation in the z direction")
    ("input,i",     po::value<std::string>()->default_value("input.obsidian"), "Input file")
    ("sample",      po::value<uint>()->default_value(0), "Number of random samples to draw")
    ("samplefile",  po::value<std::string>(), "csv file with samples")
    ("outputfile,o",po::value<std::string>()->default_value("voxels.npz"), "output file")
    ("nthreads,j",  po::value<uint>()->default_value(1), "Number of threads to use")
    ;
  return opts;
}

State parseState(const std::string& stateStr)
{
  std::vector<std::string> stateStrVec;
  boost::algorithm::split(stateStrVec,stateStr,boost::is_any_of(","));

  State state;
  constexpr uint nonSampleTerms = 5;
  const uint ndim = stateStrVec.size() - nonSampleTerms;
  state.sample.resize(ndim);
  uint index;
  for (index = 0; index < ndim; ++index)
  {
    state.sample[index] = std::stod(stateStrVec[index]);
  }
  state.energy = std::stod(stateStrVec[index]);
  state.sigma = std::stod(stateStrVec[++index]);
  state.beta = std::stod(stateStrVec[++index]);
  state.accepted = std::stoi(stateStrVec[++index]) != 0;
  state.swapType = static_cast<sl::mcmc::SwapType>(std::stoi(stateStrVec[++index]));
  return state;
}


struct VoxelAggregate
{
  VoxelAggregate(const uint nSamples, const uint nVoxels,
                 const uint nProperties, const uint nLayers)
    : nSamples(nSamples)
    , rockProps(nProperties,Eigen::VectorXd::Zero(nVoxels))
    , layers(nLayers,Eigen::VectorXd::Zero(nVoxels))
  {}

  uint nSamples;
  std::vector<Eigen::VectorXd> rockProps;
  std::vector<Eigen::VectorXd> layers;
};

VoxelAggregate process_lines(const std::vector<std::string>::const_iterator& start,
                             const std::vector<std::string>::const_iterator& end,
                             const GlobalPrior& gPrior, const GlobalSpec& gSpec,
                             const std::vector<world::InterpolatorSpec>& interp,
                             const world::Query& query)
{
  constexpr uint nProperties = static_cast<uint>(RockProperty::Count)+1;
  const uint nlayers = gSpec.world.boundaries.size();
  const uint nVoxels = query.resX * query.resY * query.resZ;

  VoxelAggregate vox(std::distance(start,end), nVoxels, nProperties, nlayers);

  for ( std::vector<std::string>::const_iterator it = start; it < end; ++it)
  {
    // parse line back into sl::mcmc::State
    State state = parseState(*it);

    const GlobalParams gParams = gPrior.reconstruct(state.sample);
    Eigen::MatrixXd transitions = world::getTransitions(interp, gParams.world, query);

    // Voxelise the mean value of each property
    for (uint j = 0; j < nProperties; j++)
    {
      // Extract only this rock property
      Eigen::VectorXd props;
      if ( j < nProperties-1 )
      {
        props = world::extractProperty(gParams.world, static_cast<RockProperty>(j));
      }
      else
      {
        props = Eigen::VectorXd::LinSpaced(nlayers,0,nlayers-1);
      }

      Eigen::MatrixXd voxels = world::voxelise(transitions, query.edgeZ, props);

      vox.rockProps[j] += world::flatten(voxels).transpose();
      // should also compute variance here
    }

    // Voxelise the probability of each layer
    for (uint j = 0; j < nlayers; j++)
    {
      // Show only this particular layer in the voxelisation
      Eigen::VectorXd props = Eigen::VectorXd::Zero(nlayers);
      props(j) = 1.0;
      Eigen::MatrixXd voxels = world::voxelise(transitions, query.edgeZ, props);

      vox.layers[j] += world::flatten(voxels).transpose();
    }
  }
  return vox;
}


VoxelAggregate marginalise(const std::vector<std::string>& lines,
                           const GlobalPrior& gPrior, const GlobalSpec& gSpec,
                           const std::vector<world::InterpolatorSpec>& interp,
                           const world::Query& query,
                           const uint nThreads)
{
  const uint n = lines.size();
  const uint m = 1 + ((n - 1) / nThreads);
  std::vector<std::string>::const_iterator start = lines.begin();

  std::vector<std::future<VoxelAggregate>> threads;

  bool done = false;
  while (!done)
  {
    std::vector<std::string>::const_iterator end;
    if (std::distance(start,lines.end()) <= m )
    {
      end = lines.end();
      done = true;
    }
    else
    {
      end = start + m;
    }

    threads.push_back( std::async(std::launch::async,process_lines,
                                  start, end, gPrior, gSpec, interp, query) );
    start = end;
  }

  constexpr uint nProperties = static_cast<uint>(RockProperty::Count)+1;
  const uint nlayers = gSpec.world.boundaries.size();
  const uint nVoxels = query.resX * query.resY * query.resZ;

  VoxelAggregate marginal(0, nVoxels, nProperties, nlayers);

  for (auto& t : threads)
  {
    VoxelAggregate vox = t.get();
    marginal.nSamples += vox.nSamples;
    for ( uint i = 0; i < nProperties; ++i )
    {
      marginal.rockProps[i] += vox.rockProps[i];
    }
    for ( uint i = 0; i < nlayers; ++i )
    {
      marginal.layers[i] += vox.layers[i];
    }
  }


  LOG(INFO) << "Divide vals by nSamples: " << marginal.nSamples << std::endl;
  for ( uint i = 0; i < nProperties; ++i )
  {
    marginal.rockProps[i] /= marginal.nSamples;
  }
  for ( uint i = 0; i < nlayers; ++i )
  {
    marginal.layers[i] /= marginal.nSamples;
  }
  marginal.nSamples = 0;

  return marginal;
}


int main(int ac, char *av[])
{
  auto vm = sl::parseCommandLine(ac, av, commandLineOptions());

  const int logLevel = vm["loglevel"].as<int>();
  sl::initLogging("obsidian-analyse", logLevel);

  sl::init::initialiseSignalHandler();

  readInputFile(vm["input"].as<std::string>(), vm);
  const uint nThreads = vm["nthreads"].as<uint>();

  // Determine which sensors are enabled from config and command line overrides
  std::set<ForwardModel> modelsConfig = parseSensorsEnabled(vm);

  // Create all data structures required for forward modelling
  const GlobalPrior gPrior = parsePrior<GlobalPrior>(vm, modelsConfig);
  const GlobalSpec gSpec = parseSpec<GlobalSpec>(vm, modelsConfig);
  //const GlobalResults gResults = loadResults(gSpec.world, vm, modelsEnabled);
  const std::vector<world::InterpolatorSpec> interp = world::worldspec2Interp(gSpec.world);
  //const GlobalCache gCache = fwd::generateGlobalCache(interp, gSpec, modelsEnabled);

  // Get the resolution
  const uint xres = vm["xres"].as<uint>();
  const uint yres = vm["yres"].as<uint>();
  const uint zres = vm["zres"].as<uint>();
  const uint nVoxels = xres * yres * zres;
  const uint nlayers = gSpec.world.boundaries.size();

  // Create the query over this grid
  world::Query query(interp, gSpec.world, xres, yres, zres, world::SamplingStrategy::noAA);

  constexpr uint nProperties = static_cast<uint>(RockProperty::Count)+1;
  std::vector<Eigen::MatrixXd> dumpProps(nProperties,Eigen::MatrixXd::Zero(1,nVoxels));
  std::vector<Eigen::MatrixXd> dumpLayers(nlayers,Eigen::MatrixXd::Zero(1,nVoxels));

  std::string filename = vm["outputfile"].as<std::string>();
  io::NpzWriter writer(filename);
  writer.write<double>("resolution", Eigen::Vector3d(query.resX, query.resY, query.resZ));
  writer.write<double>("x_bounds", Eigen::Vector2d(gSpec.world.xBounds.first, gSpec.world.xBounds.second));
  writer.write<double>("y_bounds", Eigen::Vector2d(gSpec.world.yBounds.first, gSpec.world.yBounds.second));
  writer.write<double>("z_bounds", Eigen::Vector2d(gSpec.world.zBounds.first, gSpec.world.zBounds.second));

  // Open file and count numbe rof samples
  std::ifstream ifs(vm["samplefile"].as<std::string>());
  std::vector<std::string> lines;
  for ( std::string line; std::getline(ifs,line); )
  {
    lines.push_back(line);
  }

  VoxelAggregate marginal = marginalise(lines, gPrior, gSpec, interp, query, nThreads);

  for ( uint i = 0; i < nProperties; ++i )
  {
    dumpProps[i].row(0) += marginal.rockProps[i];
  }
  for ( uint i = 0; i < nlayers; ++i )
  {
    dumpLayers[i].row(0) += marginal.layers[i];
  }

  dumpPropVoxelsNPZ(writer, dumpProps);
  dumpLayerVoxelsNPZ(writer, dumpLayers);

  return 0;
}
