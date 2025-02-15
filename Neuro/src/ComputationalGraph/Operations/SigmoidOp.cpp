#include "ComputationalGraph/Operations/SigmoidOp.h"

namespace Neuro
{
    //////////////////////////////////////////////////////////////////////////
    SigmoidOp::SigmoidOp(TensorLike* x, const string& name)
        : Operation({ x }, name.empty() ? "sigmoid" : name)
    {
        UpdateOutputShape();
    }

    //////////////////////////////////////////////////////////////////////////
    void SigmoidOp::ComputeInternal()
    {
        m_Output.ResizeBatch(m_Inputs[0]->Batch());
        m_Inputs[0]->Sigmoid(m_Output);
    }

    //////////////////////////////////////////////////////////////////////////
    void SigmoidOp::ComputeGradientInternal(const Tensor& grad)
    {
        if (m_InputNodes[0]->CareAboutGradient())
            m_Output.SigmoidGradient(m_Output, grad, m_InputsGrads[0]);
    }
}