//!
//! Contains the interface for interpolation.
//!
//! \file world/interpolatorspec.hpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <Eigen/Core>

#include "datatype/world.hpp"

namespace obsidian
{
  namespace world
  {
    //! Represents a boundary of the layer, which is specified by a mesh grid of
    //! control points that vary in depth according to a vector of parameters.
    //!
    class InterpolatorSpec
    {
      public:
        //! Initialise interpolation params.
        //!
        //! \param xBounds, yBounds, zBounds Pairs containing the min and max boundaries in each direction.
        //! \param boundarySpec The boundary specification.
        //!
        InterpolatorSpec(const std::pair<double, double>& xBounds, const std::pair<double, double>& yBounds,
                         const std::pair<double, double>& zBounds, const BoundarySpec& boundaryspec);

        //! Initialise an empty interpolator specification.
        //!
        InterpolatorSpec();

        //! Get the weights for the mean function. Multiply the transpose of this
        //! with the thetas will give the interpolated depths.
        //!
        Eigen::MatrixXd getWeights(const Eigen::MatrixXd &queryX) const;

        //! Get the number of control points.
        //!
        //! \return The total number of control points.
        //!
        int numControlPoints() const;

        //! Pair containing the x and y resolutions
        std::pair<uint, uint> resolution;

        //! 
        Eigen::MatrixXd controlPointX;
   
        //! The type of boundary that this represents
        BoundaryClass boundaryClass;

        //!
        Eigen::MatrixXd offsetFunction;

        //! 
        std::pair<double, double> offsetXBounds;

        //! 
        std::pair<double, double> offsetYBounds;

        //! 
        double floorHeight;

      private:
        // Some caching:
        Eigen::Vector2d lengthScale_;
        Eigen::LDLT<Eigen::MatrixXd> L_; // Cholesky decomposition factor
    };

  } // world namespace
} // gdf namespace
