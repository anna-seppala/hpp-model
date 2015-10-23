// Copyright (c) 2015 CNRS
// Author: Joseph Mirabel
//
//
// This file is part of hpp-model
// hpp-model is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-model is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-model  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef HPP_MODEL_EIGEN_HH
# define HPP_MODEL_EIGEN_HH

# include <hpp/model/fwd.hh>
# include <Eigen/SVD>

namespace hpp {
  namespace model {
    template <typename SVD>
      static Eigen::Ref<const typename SVD::MatrixUType>
      getU1 (const SVD& svd)
    {
      return svd.matrixU().leftCols (svd.rank());
    }

    template <typename SVD>
      static Eigen::Ref<const typename SVD::MatrixUType>
      getU2 (const SVD& svd)
    {
      return svd.matrixU().rightCols (svd.matrixU().cols() - svd.rank ());
    }

    template <typename SVD>
      static Eigen::Ref<const typename SVD::MatrixUType>
      getV1 (const SVD& svd)
    {
      return svd.matrixV().leftCols (svd.rank());
    }

    template <typename SVD>
      static Eigen::Ref<const typename SVD::MatrixUType>
      getV2 (const SVD& svd)
    {
      return svd.matrixV().rightCols (svd.matrixV().cols() - svd.rank ());
    }

    template < typename SVD>
    static void pseudoInverse(const SVD& svd,
        Eigen::Ref <typename SVD::MatrixType> pinvmat)
    {
      eigen_assert(svd.computeU() && svd.computeV() && "Eigen::JacobiSVD "
          "computation flags must be at least: ComputeThinU | ComputeThinV");

      typename SVD::SingularValuesType singularValues_inv =
        svd.singularValues().segment (0,svd.rank()).cwiseInverse ();

      pinvmat.noalias() =
        getV1<SVD> (svd) * singularValues_inv.asDiagonal() *
        getU1<SVD> (svd).adjoint();
    }

    template < typename SVD >
    void projectorOnSpan (const SVD svd,
        Eigen::Ref <typename SVD::MatrixType> projector)
    {
      eigen_assert(svd.computeU() && svd.computeV() && "Eigen::JacobiSVD "
          "computation flags must be at least: ComputeThinU | ComputeThinV");

      projector.noalias() = getV1<SVD> (svd) * getV1<SVD>(svd).adjoint();
    }

    template < typename SVD >
    void projectorOnSpanOfInv (const SVD svd,
        Eigen::Ref <typename SVD::MatrixType> projector)
    {
      eigen_assert(svd.computeU() && svd.computeV() && "Eigen::JacobiSVD "
          "computation flags must be at least: ComputeThinU | ComputeThinV");

      projector.noalias() = getU1<SVD>(svd) * getU1<SVD>(svd).adjoint();
    }

    template < typename SVD >
    void projectorOnKernel (const SVD svd,
        Eigen::Ref <typename SVD::MatrixType> projector,
        const bool& computeFullV = false)
    {
      eigen_assert(svd.computeV() && "Eigen::JacobiSVD "
          "computation flags must be at least: ComputeThinV");

      if (computeFullV)
        projector.noalias() = getV2<SVD> (svd) * getV2<SVD>(svd).adjoint();
      else {
        projector.noalias() = - getV1<SVD> (svd) * getV1<SVD>(svd).adjoint();
        projector.diagonal().noalias () += vector_t::Ones(svd.matrixV().rows());
      }
    }

    template < typename SVD >
    void projectorOnKernelOfInv (const SVD svd,
        Eigen::Ref <typename SVD::MatrixType> projector,
        const bool& computeFullU = false)
    {
      eigen_assert(svd.computeU() && "Eigen::JacobiSVD "
          "computation flags must be at least: ComputeThinU");

      if (computeFullU) {
        // U2 * U2*
        projector.noalias() = getU2<SVD>(svd) * getU2<SVD>(svd).adjoint();
      } else {
        // I - U1 * U1*
        projector.noalias() = - getU1<SVD>(svd) * getU1<SVD>(svd).adjoint();
        projector.diagonal().noalias () += vector_t::Ones(svd.matrixU().rows());
      }
    }
  } // namespace model
} // namespace hpp

#endif // HPP_MODEL_EIGEN_HH
