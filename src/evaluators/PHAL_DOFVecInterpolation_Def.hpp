//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include "Teuchos_TestForException.hpp"
#include "Phalanx_DataLayout.hpp"

#include "Intrepid_FunctionSpaceTools.hpp"

namespace PHAL {

//**********************************************************************
template<typename EvalT, typename Traits>
DOFVecInterpolation<EvalT, Traits>::
DOFVecInterpolation(const Teuchos::ParameterList& p,
                    const Teuchos::RCP<Albany::Layouts>& dl) :
  val_node    (p.get<std::string>  ("Variable Name"), dl->node_vector),
  BF          (p.get<std::string>  ("BF Name"),  dl->node_qp_scalar),
  val_qp      (p.get<std::string>  ("Variable Name"), dl->qp_vector)
{
  this->addDependentField(val_node);
  this->addDependentField(BF);
  this->addEvaluatedField(val_qp);

  this->setName("DOFVecInterpolation"+PHX::TypeString<EvalT>::value);
  std::vector<PHX::DataLayout::size_type> dims;
  BF.fieldTag().dataLayout().dimensions(dims);
  numNodes = dims[1];
  numQPs   = dims[2];

  val_node.fieldTag().dataLayout().dimensions(dims);
  vecDim   = dims[2];
}

//**********************************************************************
template<typename EvalT, typename Traits>
void DOFVecInterpolation<EvalT, Traits>::
postRegistrationSetup(typename Traits::SetupData d,
                      PHX::FieldManager<Traits>& fm)
{
  this->utils.setFieldData(val_node,fm);
  this->utils.setFieldData(BF,fm);
  this->utils.setFieldData(val_qp,fm);
}

//**********************************************************************
template<typename EvalT, typename Traits>
void DOFVecInterpolation<EvalT, Traits>::
evaluateFields(typename Traits::EvalData workset)
{
  // This is needed, since evaluate currently sums into
  for (int i=0; i < val_qp.size() ; i++) val_qp[i] = 0.0;

  for (std::size_t cell=0; cell < workset.numCells; ++cell) {
    for (std::size_t node=0; node < numNodes; ++node) {
      for (std::size_t qp=0; qp < numQPs; ++qp) {
        for (std::size_t i=0; i<vecDim; i++) {
          val_qp(cell,qp,i) += val_node(cell, node, i) * BF(cell, node, qp);
        } 
      } 
    } 
  }
//  Intrepid::FunctionSpaceTools::evaluate<ScalarT>(val_qp, val_node, BF);
}

//**********************************************************************
}
