// FractalViewer.hpp
#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include "SampleBase.hpp"
#include "BasicMath.hpp"
#include "ThreadSignal.hpp"
#include "MapHelper.hpp"
#include "float.h"

namespace Diligent
{

    struct Vertex
    {
        float3 Pos;
        float2 UV;
    };

    class FractalViewer final : public SampleBase
    {
    public:
        void Initialize(const SampleInitInfo& InitInfo) override final;
        void Render() override final;
        void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final;

        const Char* GetSampleName() const override final { return "Fractal Viewer"; }

    private:
        // Pipeline state and resources
        RefCntAutoPtr<IPipelineState>      m_pPSO;
    };

} // namespace Diligent
