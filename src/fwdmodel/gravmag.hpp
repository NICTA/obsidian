//!
//! Contains common interface used by both gravity and magnetic forward models.
//!
//! \file fwdmodel/gravmag.hpp
//! \author Darren Shen
//! \author Alistair Reid
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Dense>
#include "world/voxelise.hpp"

namespace obsidian
{
  namespace fwd
  {
    //! Structure containing gravity and magnetic interpolation parameters.
    //!
    struct GravmagInterpolatorParams
    {
      //! The indices of each sensor.
      Eigen::MatrixXi sensorIndices;

      //! The weights of each sensor.
      Eigen::MatrixXd sensorWeights;

      //! The locations of each sensor.
      Eigen::MatrixXd gridLocations;
    };

    //! Create a GravmagInterpolatorParams object.
    //!
    //! \param voxelisation The voxelised world.
    //! \param locations The sensor locations.
    //! \param worldSpec The world specifications.
    //!
    GravmagInterpolatorParams makeInterpParams(const VoxelSpec& voxelisation, const Eigen::MatrixXd& locations, const WorldSpec& worldSpec);

    //! Computes the field values for either gravity or magnetic.
    //!
    //! \param sens The gravity or magnetic sensitivity matrix.
    //! \param sensorIndices The indices of each of the sensors.
    //! \param sensorWeights The weights of each of the sensors.
    //! \param properties The rock property for the sensor type.
    //!
    Eigen::VectorXd computeField(const Eigen::MatrixXd &sens, const Eigen::MatrixXi sensorIndices, const Eigen::MatrixXd sensorWeights,
                                 const Eigen::VectorXd &properties);

    namespace detail
    {
      //! A small number added to denominators to prevent them from being zero.
      //!
      const double EPS = 1e-12;

      //! Computes the sensitivity for either gravity or magnetic.
      //!
      //! \param xEdges, yEdges, zEdges The coordinates of the mesh grid in
      //!                               the x, y, and z directions.
      //! \param xField, yField, zField The geological field values at each of the
      //!                               mesh grid points.
      //! \param locations A Nx3 matrix containing the coordinates of each.
      //!                  sensor observation location.
      //! \param sensFunc The function to compute the sensitivity.
      //!
      Eigen::MatrixXd computeSensitivity(const Eigen::VectorXd &xEdges, const Eigen::VectorXd &yEdges, const Eigen::VectorXd &zEdges,
                                         const Eigen::VectorXd &xField, const Eigen::VectorXd &yField, const Eigen::VectorXd &zField,
                                         const Eigen::MatrixXd &locations,
                                         double (*sensFunc)(double, double, double, double, double, double));
    } // namespace detail
  } // namespace fwd
} // namespace obsidian
