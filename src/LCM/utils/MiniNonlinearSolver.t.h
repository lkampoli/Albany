//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

namespace LCM
{

template<typename EvalT, typename Traits, Intrepid::Index N>
NewtonSolver_Base<EvalT, Traits, N>::NewtonSolver_Base()
{
  return;
}

//
// Specializations
//

//
// Residual
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::Residual, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::Residual, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::Residual, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
inline
NewtonSolver<PHAL::AlbanyTraits::Residual, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// Jacobian
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::Jacobian, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::Jacobian, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::Jacobian, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::Jacobian, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// Tangent
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::Tangent, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::Tangent, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::Tangent, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::Tangent, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// DistParamDeriv
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::DistParamDeriv, Traits, N>
::NewtonSolver()
: NewtonSolver_Base<PHAL::AlbanyTraits::DistParamDeriv, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::DistParamDeriv, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::DistParamDeriv, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

#ifdef ALBANY_SG
//
// SGResidual
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::SGResidual, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::SGResidual, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::SGResidual, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
inline
NewtonSolver<PHAL::AlbanyTraits::SGResidual, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// SGJacobian
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::SGJacobian, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::SGJacobian, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::SGJacobian, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::SGJacobian, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// SGTangent
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::SGTangent, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::SGTangent, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::SGTangent, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::SGTangent, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}
#endif

#ifdef ALBANY_ENSEMBLE
//
// MPResidual
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::MPResidual, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::MPResidual, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::MPResidual, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
inline
NewtonSolver<PHAL::AlbanyTraits::MPResidual, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// MPJacobian
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::MPJacobian, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::MPJacobian, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::MPJacobian, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::MPJacobian, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

//
// MPTangent
//
template<typename Traits, Intrepid::Index N>
NewtonSolver<PHAL::AlbanyTraits::MPTangent, Traits, N>::NewtonSolver() :
    NewtonSolver_Base<PHAL::AlbanyTraits::MPTangent, Traits, N>()
{
  return;
}

template<typename Traits, Intrepid::Index N>
template <typename Residual>
void
inline
NewtonSolver<PHAL::AlbanyTraits::MPTangent, Traits, N>::
solve(
    Residual const & residual,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}

template<typename Traits, Intrepid::Index N>
void
NewtonSolver<PHAL::AlbanyTraits::MPTangent, Traits, N>::
computeFadInfo(
    Intrepid::Tensor<ScalarT, N> const & A,
    Intrepid::Vector<ScalarT, N> const & b,
    Intrepid::Vector<ScalarT, N> & x)
{
  return;
}
#endif

} // namespace LCM
