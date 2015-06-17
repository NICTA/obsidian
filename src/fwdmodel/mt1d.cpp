//!
//! Contains the implementation for the 1D MT forward model.
//!
//! \file fwdmodel/mt1d.cpp
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "mt1d.hpp"
#include "world/transitions.hpp"
#include "world/voxelise.hpp"
#include "world/property.hpp"

#include <glog/logging.h>

namespace obsidian
{
  namespace fwd
  {
    namespace detail
    {
      Eigen::Matrix2cd applyRotation(const Eigen::Matrix2cd &Z, double phaseBelow, double phaseAbove)
      {
        // Calculate the rotation matrix
        auto theta = phaseBelow - phaseAbove;
        auto c = std::cos(theta), s = std::sin(theta);
        Eigen::Matrix2d R;
        R << c, s, -s, c;

        // Apply the rotation matrix
        return R * Z * R.transpose();
      }

      double primaryAngle(double y, double x)
      {
        if (fabs(x) < 1e-10)
          return M_PI / 2.0;
        return atan(y / x);
      }
    }

    Eigen::MatrixX4d phaseTensor1d(const Eigen::MatrixX4cd &Z)
    {
      Eigen::MatrixXd tensor(Z.rows(), 4);

      for (uint i = 0; i < Z.rows(); i++)
      {
        std::complex<double> zii = Z(i, 0), zij = Z(i, 1), zji = Z(i, 2), zjj = Z(i, 3);

        tensor.row(i) << zjj.real() * zii.imag() - zij.real() * zji.imag(), zjj.real() * zij.imag() - zij.real() * zjj.imag(), zii.real()
            * zji.imag() - zji.real() * zii.imag(), zii.real() * zjj.imag() - zji.real() * zij.imag();

        tensor.row(i) /= zii.real() * zjj.real() - zji.real() * zij.real();
      }

      return tensor;
    }

    Eigen::VectorXd alpha1d(const Eigen::MatrixX4d &tensor)
    {
      Eigen::VectorXd alpha(tensor.rows());
      for (int i = 0; i < tensor.rows(); i++)
      {
        alpha(i) = detail::primaryAngle(tensor(i, 1) + tensor(i, 2), tensor(i, 0) - tensor(i, 3)) / 2.0;
      }
      return alpha;
    }

    Eigen::VectorXd beta1d(const Eigen::MatrixX4d &tensor)
    {
      Eigen::VectorXd beta(tensor.rows());
      for (int i = 0; i < beta.rows(); i++)
      {
        beta(i) = detail::primaryAngle(tensor(i, 1) - tensor(i, 2), tensor(i, 0) + tensor(i, 3)) / 2.0;
      }
      return beta;
    }

    Eigen::MatrixX4cd impedenceAniso1d(const Eigen::VectorXd &thicknesses, const Eigen::VectorXd &freqs, const Eigen::VectorXd &resx,
                                       const Eigen::VectorXd &resy, const Eigen::VectorXd &phases)
    {
      int nlayers = thicknesses.rows();

      // The impedance matrix to be filled
      Eigen::MatrixX4cd zz(freqs.rows(), 4);

      for (int i = 0; i < freqs.rows(); i++)
      {
        // Angular frequency
        double w = 2 * M_PI * freqs(i);

        // Start at the bottom layer
        auto zxp = std::sqrt(detail::MU * w * resx[nlayers - 1]) * std::exp(detail::I * M_PI / 4.0);
        auto zyp = std::sqrt(detail::MU * w * resy[nlayers - 1]) * std::exp(detail::I * M_PI / 4.0);

        Eigen::Matrix2cd Z;
        Z << 0.0, zxp, -zyp, 0.0;

        // Rotate to the next layer's angle
        Z = detail::applyRotation(Z, phases[nlayers - 2], 0);

        std::complex<double> zii = Z(0, 0), zij = Z(0, 1), zji = Z(1, 0), zjj = Z(1, 1);

        // Go through each layer from bottom to top
        for (int j = nlayers - 2; j >= 0; j--)
        {
          double th = thicknesses(j);

          auto ki = std::sqrt(detail::MU * w / resx(j)) * std::exp(-detail::I * M_PI / 4.0);
          auto kj = std::sqrt(detail::MU * w / resy(j)) * std::exp(-detail::I * M_PI / 4.0);

          auto zpll = detail::MU * w / ki;
          auto zprp = -detail::MU * w / kj;

          auto phii = zii * zjj / (zij + zpll);
          auto phij = zii * zjj / (zji + zprp);

          auto rj = (zji - zprp - phii) / (zji + zprp - phii);
          auto ri = (zij - zpll - phij) / (zij + zpll - phij);

          auto lj = 2.0 * zpll * zjj / ((zji + zprp) * (zij + zpll - phij));
          auto li = 2.0 * zprp * zii / ((zij + zpll) * (zji + zprp - phii));

          auto l = li * lj * std::exp(detail::I * -2.0 * (ki + kj) * th);

          auto eri = ri * std::exp(detail::I * -2.0 * ki * th);
          auto erj = rj * std::exp(detail::I * -2.0 * kj * th);
          auto ekk = std::exp(detail::I * -(ki + kj) * th);

          // New impedances
          zii = 2.0 * li * zpll * ekk / ((1.0 - erj) * (1.0 - eri) - l);
          zij = zpll * (((1.0 + eri) * (1.0 - erj) + l) / ((1.0 - eri) * (1.0 - erj) - l));
          zji = zprp * (((1.0 + erj) * (1.0 - eri) + l) / ((1.0 - erj) * (1.0 - eri) - l));
          zjj = 2.0 * lj * zprp * ekk / ((1.0 - erj) * (1.0 - eri) - l);

          Z << zii, zij, zji, zjj;

          // Rotate to the orientation of the next layer
          if (j > 0)
          {
            Z = detail::applyRotation(Z, phases(j - 1), phases(j));
          } else
          {
            // Handle top layer
            Z = detail::applyRotation(Z, 0, phases(j)) * 1e-3 / detail::MU;
          }

          zii = Z(0, 0);
          zij = Z(0, 1);
          zji = Z(1, 0);
          zjj = Z(1, 1);
        }

        zz.row(i) << zii, zij, zji, zjj;
      }

      return zz;
    }

    //! Generate a cache object for a MT forward model.
    //!
    //! \param boundaryInterpolation The world model interpolation parameters.
    //! \param worldSpec The world model specification.
    //! \param mtSpec The forward model specification.
    //! \returns Forward model cache object.
    //!
    template<>
    MtAnisoCache generateCache<ForwardModel::MTANISO>(const std::vector<world::InterpolatorSpec>& boundaryInterpolation,
                                                      const WorldSpec& worldSpec, const MtAnisoSpec& mtSpec)
    {
      LOG(INFO)<< "Caching mtaniso sensitivity...";
      return
      {
        boundaryInterpolation,
        world::Query(boundaryInterpolation, worldSpec, mtSpec.locations.leftCols(2))
      };
    }

    //! Run a MT forward model.
    //! 
    //! \param spec The forward model specification.
    //! \param cache The forward model cache generated by generateCache().
    //! \param world The world model parameters.
    //! \returns Forward model results.
    //!
    template<>
    MtAnisoResults forwardModel<ForwardModel::MTANISO>(const MtAnisoSpec& spec, const MtAnisoCache& cache, const WorldParams& world)
    {
      Eigen::MatrixXd transitions = world::getTransitions(cache.boundaryInterpolation, world, cache.query);
      Eigen::MatrixXd thicknesses = world::thickness(transitions); // mqueries x nthicknesses

      Eigen::VectorXd resx = world::extractProperty(world, RockProperty::ResistivityX);
      Eigen::VectorXd resy;
      Eigen::VectorXd phase;
      if (spec.ignoreAniso)
      {
        resy = resx;
        phase = Eigen::VectorXd::Zero(resy.size());
      } else
      {
        resy = world::extractProperty(world, RockProperty::ResistivityY);
        phase = world::extractProperty(world, RockProperty::ResistivityPhase);
      }

      MtAnisoResults results;
      for (uint i = 0; i < thicknesses.rows(); i++)
      {
        results.readings.push_back(impedenceAniso1d(thicknesses.row(i), spec.freqs[i], resx, resy, phase));
        results.phaseTensor.push_back(phaseTensor1d(results.readings[i]));
        results.alpha.push_back(alpha1d(results.phaseTensor[i]));
        results.beta.push_back(beta1d(results.phaseTensor[i]));
      }
      return results;
    }
  }
}
