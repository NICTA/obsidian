//!
//! Contains the implementation for creating grids.
//!
//! \file world/grid.cpp
//! \author Alistair Reid
//! \author Darren Shen
//! \date 2014
//! \license Affero General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "world/grid.hpp"

#include <Eigen/Dense>
#include <Eigen/Core>

namespace obsidian
{
  namespace world
  {
    Eigen::MatrixXd sensorGrid(const WorldSpec& worldSpec, uint resx, uint resy, double sensorZ)
    {
        Eigen::MatrixXd xyLocations = world::internalGrid2D(worldSpec.xBounds, worldSpec.yBounds, resx, resy);
        Eigen::MatrixXd xyzLocations = Eigen::MatrixXd(xyLocations.rows(), 3);
        xyzLocations.leftCols<2>() = xyLocations;
        xyzLocations.col(2).fill(sensorZ);
        return xyzLocations;
    }

    Eigen::MatrixXd internalGrid2D(std::pair<double,double> xMinMax, std::pair<double,double> yMinMax, uint resx, uint resy)
    {
      double xmin = xMinMax.first;
      double xmax = xMinMax.second;
      double ymin = yMinMax.first;
      double ymax = yMinMax.second;
      double deltax = xmax - xmin;
      double deltay = ymax - ymin;
      Eigen::MatrixXd xyLocations(resx*resy, 2);
      uint c = 0;
      for (uint j = 0; j < resy; j++)
      {
        for (uint i = 0; i < resx; i++)
        {
          double xLoc = xmin + deltax * ((double)i + 0.5) / (double)resx;
          double yLoc = ymin + deltay * ((double)j + 0.5) / (double)resy;
          xyLocations(c,0) = xLoc;
          xyLocations(c,1) = yLoc;
          c++;
        }
      }

      return xyLocations;
    }

    Eigen::MatrixXd internalGrid2DX(std::pair<double,double> xMinMax, std::pair<double,double> yMinMax, uint resx, uint resy)
    {
      double xmin = xMinMax.first;
      double xmax = xMinMax.second;
      double ymin = yMinMax.first;
      double ymax = yMinMax.second;
      double deltax = xmax - xmin;
      double deltay = ymax - ymin;
      Eigen::MatrixXd xyLocations(resx*resy, 2);
      uint c = 0;

      for (uint i = 0; i < resx; i++)
      {
        for (uint j = 0; j < resy; j++)
        {
          double xLoc = xmin + deltax * ((double)i + 0.5) / (double)resx;
          double yLoc = ymin + deltay * ((double)j + 0.5) / (double)resy;
          xyLocations(c,0) = xLoc;
          xyLocations(c,1) = yLoc;
          c++;
        }
      }

      return xyLocations;
    }

    Eigen::MatrixXd edgeGrid2D(std::pair<double, double> xMinMax, std::pair<double, double> yMinMax,
        uint resx, uint resy)
    {
      double xmin = xMinMax.first;
      double xmax = xMinMax.second;
      double ymin = yMinMax.first;
      double ymax = yMinMax.second;
      double deltax = xmax - xmin;
      double deltay = ymax - ymin;
      Eigen::MatrixXd xyLocations(resx*resy, 2);
      uint c = 0;
      for (uint j = 0; j < resy; j++)
      {
        for (uint i = 0; i < resx; i++)
        {
          double xLoc = xmin + deltax * (double)i/(double)(resx-1);
          double yLoc = ymin + deltay * (double)j/(double)(resy-1);
          xyLocations(c,0) = xLoc;
          xyLocations(c,1) = yLoc;
          c++;
        }
      }

      return xyLocations;
    }

    Eigen::MatrixXd sensorGrid3d(const WorldSpec& worldSpec, uint resx, uint resy, uint resz)
    {
      double xmin = worldSpec.xBounds.first;
      double xmax = worldSpec.xBounds.second;
      double ymin = worldSpec.yBounds.first;
      double ymax = worldSpec.yBounds.second;
      double zmin = worldSpec.zBounds.first;
      double zmax = worldSpec.zBounds.second;
      double deltax = xmax - xmin;
      double deltay = ymax - ymin;
      double deltaz = zmax - zmin;

      Eigen::MatrixXd xyzLocations(resx*resy*resz, 3);
      uint c = 0;
      for (uint i = 0; i < resx; i++)
      {
        for (uint j = 0; j < resy; j++)
        {
          for (uint k = 0; k < resz; k++)
          {
            double xLoc = xmin + deltax * ((double)i + 0.5) / (double)resx;
            double yLoc = ymin + deltay * ((double)j + 0.5) / (double)resy;
            double zLoc = zmin + deltaz * ((double)k + 0.5) / (double)resz;
            xyzLocations(c,0) = xLoc;
            xyzLocations(c,1) = yLoc;
            xyzLocations(c,2) = zLoc;
            c++;
          }
        }
      }

      return xyzLocations;
    }

    double length(const Eigen::Vector2d &pt)
    {
      return pt(1) - pt(0);
    }

    Eigen::VectorXd linrange(double from, double to, double step)
    {
      int size = std::floor((to - from) / step) + 1;
      Eigen::VectorXd result(size);
      for (int i = 0; i < result.rows(); i++)
        result(i) = from + (i * step);
      return result;
    }

    std::pair<Eigen::MatrixXd, Eigen::MatrixXd> meshgrid(const Eigen::VectorXd &x, const Eigen::VectorXd &y)
    {
      // Courtesy of: http://forum.kde.org/viewtopic.php?f=74&t=90876
      return std::pair<Eigen::MatrixXd, Eigen::MatrixXd>(x.transpose().replicate(y.rows(), 1), y.replicate(1, x.rows()));
    }

    Eigen::VectorXd flatten(const Eigen::MatrixXd& matrix)
    {
      Eigen::MatrixXd res(matrix);
      res.resize(matrix.rows() * matrix.cols(), 1);
      return res;
    }

    Eigen::VectorXi flatten(const Eigen::MatrixXi &matrix)
    {
      Eigen::MatrixXi res(matrix);
      res.resize(matrix.rows() * matrix.cols(), 1);
      return res;
    }

    std::pair<Eigen::MatrixXd, std::pair<Eigen::MatrixXd, Eigen::MatrixXd>>
    makeGrid(const Eigen::Vector2d &boundaryStart, const Eigen::Vector2d &boundaryEnd,
      const Eigen::Vector2i &resolution, int padding)
    {
      padding = 0;

      // assert(spacing.min() > 0)
      Eigen::Vector2d spacing = (boundaryEnd - boundaryStart).array() /
        (resolution.array() - 1 - 2 * padding).cast<double>();

      // Handle 1 control point using the midpoint of the boundaries
      Eigen::VectorXd linX = resolution(0) == 1 ?
        Eigen::VectorXd::Ones(1) * ((boundaryStart(0) + boundaryEnd(0)) / 2) :
        linrange(boundaryStart(0) - spacing(0) * padding, boundaryEnd(0) + spacing(0) * padding,
          spacing(0));

      Eigen::VectorXd linY = resolution(1) == 1 ?
        Eigen::VectorXd::Ones(1) * ((boundaryStart(1) + boundaryEnd(1)) / 2) :
        linrange(boundaryStart(1) - spacing(1) * padding, boundaryEnd(1) + spacing(1) * padding,
          spacing(1));

      // Get the mesh grid
      auto mesh = meshgrid(linX, linY);
      
      // We return the mesh as a N*2 matrix, where the first column contains 
      // the values of the flattened X coordinates and second column contains
      // the flattened Y coordinates.
      Eigen::MatrixXd flattenedMesh(linX.rows() * linY.rows(), 2);
      flattenedMesh.col(0) << flatten(mesh.first);
      flattenedMesh.col(1) << flatten(mesh.second);

      return std::make_pair(flattenedMesh, std::make_pair(linX, linY));
    }
  }
}
