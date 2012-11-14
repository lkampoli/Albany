//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#ifndef SCALAR_L2_PROJECTION_RESIDUAL_HPP
#define SCALAR_L2_PROJECTION_RESIDUAL_HPP

#include "Phalanx_ConfigDefs.hpp"
#include "Phalanx_Evaluator_WithBaseImpl.hpp"
#include "Phalanx_Evaluator_Derived.hpp"
#include "Phalanx_MDField.hpp"

#include "Intrepid_CellTools.hpp"
#include "Intrepid_Cubature.hpp"

namespace LCM {
/** \brief Finite Element Interpolation Evaluator

    This evaluator computes residual of a scalar projection from Gauss points to nodes.

*/

template<typename EvalT, typename Traits>
class ScalarL2ProjectionResidual : public PHX::EvaluatorWithBaseImpl<Traits>,
				public PHX::EvaluatorDerived<EvalT, Traits>  {

public:

  ScalarL2ProjectionResidual(const Teuchos::ParameterList& p);

  void postRegistrationSetup(typename Traits::SetupData d,
                      PHX::FieldManager<Traits>& vm);

  void evaluateFields(typename Traits::EvalData d);

private:

  typedef typename EvalT::ScalarT ScalarT;
  typedef typename EvalT::MeshScalarT MeshScalarT;

  // Input:
  PHX::MDField<MeshScalarT,Cell,Node,QuadPoint> wBF;
  PHX::MDField<MeshScalarT,Cell,Node,QuadPoint,Dim> wGradBF;
  PHX::MDField<ScalarT,Cell,QuadPoint,Dim, Dim> DefGrad;
  PHX::MDField<ScalarT,Cell,QuadPoint> projectedStress;


  // Input for hydro-static stress effect
  PHX::MDField<ScalarT,Cell,QuadPoint,Dim,Dim> Pstress;







  bool haveSource;
  bool haveMechSource;
  bool enableTransient;

  unsigned int numNodes;
  unsigned int numQPs;
  unsigned int numDims;
  unsigned int worksetSize;


  Intrepid::FieldContainer<ScalarT> tauStress;
  Intrepid::FieldContainer<ScalarT> tauH;

  // Output:
  PHX::MDField<ScalarT,Cell,Node> TResidual;


};
}

#endif
