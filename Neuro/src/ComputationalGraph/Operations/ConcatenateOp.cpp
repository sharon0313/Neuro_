#include "ComputationalGraph/Operations/ConcatenateOp.h"

namespace Neuro
{
    //////////////////////////////////////////////////////////////////////////
    ConcatenateOp::ConcatenateOp(const vector<TensorLike*>& xs, EAxis axis, const string& name)
        : Operation(xs, name.empty() ? "concatenate" : name), m_Axis(axis)
    {
        UpdateOutputShape();
    }

    //////////////////////////////////////////////////////////////////////////
    void ConcatenateOp::UpdateOutputShape()
    {
        auto sumDims = [](const vector<TensorLike*>& inputs, size_t dim)
        {
            uint32_t sum = 0;
            for (auto x : inputs)
            {
                for (int a = _0Axis; a <= _3Axis; ++a)
                {
                    if (a == dim)
                        sum += x->GetShape().Len(dim);
                    else
                        NEURO_ASSERT(x->GetShape().Len(a) == inputs[0]->GetShape().Len(a), "All inputs must have the same dimensions except for the concatenated one.");
                }
            }
            return sum;
        };

        const auto& shape = m_InputNodes[0]->GetShape();

        if (m_Axis == WidthAxis)
            m_Output.Resize(Shape(sumDims(m_InputNodes, 0), shape.Height(), shape.Depth(), shape.Batch()));
        else if (m_Axis == HeightAxis)
            m_Output.Resize(Shape(shape.Width(), sumDims(m_InputNodes, 1), shape.Depth(), shape.Batch()));
        else if (m_Axis == DepthAxis)
            m_Output.Resize(Shape(shape.Width(), shape.Height(), sumDims(m_InputNodes, 2), shape.Batch()));
        else if (m_Axis == BatchAxis)
            m_Output.Resize(Shape(shape.Width(), shape.Height(), shape.Depth(), (uint32_t)m_InputNodes.size()));
        else
            assert(false);
    }

    //////////////////////////////////////////////////////////////////////////
    void ConcatenateOp::ComputeInternal()
    {
        if (m_Axis == BatchAxis)
            m_Output.ResizeBatch((uint32_t)m_Inputs.size());
        else
            m_Output.ResizeBatch(m_Inputs[0]->Batch());
        Tensor::Concat(m_Axis, m_Inputs, m_Output);
    }

    //////////////////////////////////////////////////////////////////////////
    void ConcatenateOp::ComputeGradientInternal(const Tensor& grad)
    {
        bool anyInputCareAboutGrad = false;
        for (auto inputNode : m_InputNodes)
        {
            if (inputNode->CareAboutGradient())
            {
                anyInputCareAboutGrad = true;
                break;
            }
        }

        if (anyInputCareAboutGrad)
            grad.Split(m_Axis, m_InputsGradsPtrs);
    }

    //////////////////////////////////////////////////////////////////////////
    bool ConcatenateOp::ForceAllocInputGradNode(size_t index) const
    {
        bool anyInputCareAboutGrad = false;
        for (auto inputNode : m_InputNodes)
        {
            if (inputNode->CareAboutGradient())
            {
                anyInputCareAboutGrad = true;
                break;
            }
        }

        // we cannot compute input gradient per input separately so if any input needs gradient we have to allocate them all
        return anyInputCareAboutGrad;
    }
}