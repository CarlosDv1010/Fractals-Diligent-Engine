#include "FractalViewer.hpp"

namespace Diligent
{

    void FractalViewer::Initialize(const SampleInitInfo& InitInfo)
    {
        SampleBase::Initialize(InitInfo);

        // Aquí luego se configurará el pipeline, shaders, etc.
    }

    void FractalViewer::Render()
    {
        const float ClearColor[] = { 0.1f, 0.1f, 0.15f, 1.0f };

        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();

        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Aquí luego irán los draw calls
    }

    void FractalViewer::Update(double CurrTime, double ElapsedTime, bool DoUpdateUI)
    {
        SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);
        // Aquí luego se actualizará la cámara, tiempo, etc.
    }

} // namespace Diligent
