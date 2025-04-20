#pragma once

#include "SampleBase.hpp"
#include "BasicMath.hpp"

namespace Diligent
{

    class FractalViewer final : public SampleBase
    {
    public:
        virtual void Initialize(const SampleInitInfo& InitInfo) override final;

        virtual void Render() override final;
        virtual void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final;

        virtual const Char* GetSampleName() const override final { return "Tutorial02: Cube"; }

    private:
        void CreatePipelineState();
        void CreateVertexBuffer();
		void CreateIndexBuffer();

        RefCntAutoPtr<IPipelineState>         m_pPSO;
        RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
        RefCntAutoPtr<IBuffer>                m_VertexBuffer;
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
        RefCntAutoPtr<IBuffer>                m_VSConstants;
		float m_Time = 0.f;
    };

} // namespace Diligent