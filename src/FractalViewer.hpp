#pragma once

#include "SampleBase.hpp"
#include "BasicMath.hpp"
#include "FirstPersonCamera.hpp"

namespace Diligent
{

    class FractalViewer final : public SampleBase
    {
    public:
        virtual void Initialize(const SampleInitInfo& InitInfo) override final;

        virtual void Render() override final;
        virtual void Update(double CurrTime, double ElapsedTime, bool DoUpdateUI) override final;

        virtual const Char* GetSampleName() const override final { return "Tutorial02: Cube"; }
        virtual void WindowResize(Uint32 Width, Uint32 Height) override;


    protected:
		virtual void UpdateUI() override final;


    private:
        void CreatePipelineState();
        void CreateVertexBuffer();
        void CreateComputePipelineState();
        void CreateQuadPipelineState();
		void CreateIndexBuffer();

        enum class RenderMode
        {
            PixelShader = 0,
            ComputeShader
        };

        struct ShaderConstants
        {
            float4 TimeAndResolution; // x=time, y=res.x, z=res.y, w=fractType
            float4 CameraPos; // xyz=pos, w=is3D
            float4 CameraDirX; // xyz=right
            float4 CameraDirY; // xyz=up
            float4 CameraDirZ; // xyz=forward

            float4 ZoomOffset; // x=zoom/size, y=off.x, z=off.y, w=off.z
            float4 FractalColor; // rgba tint
            float4 BackgroundColor; // rgba background

            float4 FractalC; // x=c.x, y=c.y
            int maxiter; // Usado para Menger iterations
            float3 FractalParams1; // x=bailout(unused), y=power_mandelbulb_min, z=unused
            float4 FractalParams2; // x=gamma(unused), y/z/w extras

            float4 Options3D; // x=maxSteps, y=maxDist, z=threshold, w=unused
            float4 AnimationParams; // x=timeScale, y=unused, z=unused, w=unused

            float4 LightPositionAndRadius; // xyz=pos, w=radius
            float4 LightColorAndIntensity; // rgb=color, a=intensity

            float aoStrengthMultiplier;
            float directLightBrightness;
            float ambientLightBrightness;
            float reflectivityFactor;
            float fresnelStrength; // Potencia del exponente Fresnel
            float normalEpsilonScale; // Escala para el epsilon de cálculo de normales (ej. 0.5)
            int bounces; // Escala de textura UV (x=scaleX, y=scaleY)
            float padding;
        };

        RenderMode m_RenderMode = RenderMode::PixelShader;
    
        RefCntAutoPtr<ITexture> m_pComputeOutputTex;

        RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        RefCntAutoPtr<IPipelineState>         m_pComputePSO;
        RefCntAutoPtr<IPipelineState>         m_pQuadPSO;
        RefCntAutoPtr<IPipelineState>         m_pPSO;
        RefCntAutoPtr<IShaderResourceBinding> m_pComputeSRB;
        RefCntAutoPtr<IShaderResourceBinding> m_pQuadSRB;
        RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
        RefCntAutoPtr<IBuffer>                m_VertexBuffer;
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
        RefCntAutoPtr<IBuffer>                m_VSConstants;
        RefCntAutoPtr<IBuffer>                m_VSConstantsComputeShader;
        int    m_SelectedFractal2D = 0;      
        int    m_SelectedFractal3D = 0;          
        bool   first_timeUI = true;

        // Variables de configuración del fractal
        float  m_Time = 0.0f;      
        int    m_FractalType = 0;          
        FirstPersonCamera m_Camera;              
        bool              m_is3D = false;
        bool m_usesComputePipeline = false;
        float m_Zoom = 1.0f;   
        float m_OffsetX = 0.0f, m_OffsetY = 0.0f, m_OffsetZ = 0.0f;
        float4 m_FractalColor = float4{ 1,1,1,1 }; 
        float4 m_BackgroundColor = float4{ 0,0,0,1 }; 
        float4 m_FractalC = float4{ 0,0,0,0 };
        int m_maxiter = 100; 
        float3 m_FractalParams1 = float3{ 2.0f, 2.0f, 0.0f };
        float4 m_FractalParams2 = float4{ 1.0f, 0, 0, 0 };
        float4 m_Options3D = float4{ 0,0,0,0 };
        float4 m_AnimationParams = float4{ 1.0f,0,0,0 };
        float m_aoStrengthMultiplier = 1.3f;
        float m_directLightBrightness = 1.5f;
        float m_ambientLightBrightness = 0.2f;
        float m_reflectivityFactor = 0.7f;
        float m_fresnelStrength = 4.0f;
        float m_normalEpsilonScale = 0.5f;
		float2 m_textureScale = float2{ 1.0f, 1.0f };
        float3 m_LightPosition = { -2.0f, 2.0f, -3.0f };
        float            m_LightRadius = 0.2f;
        float3  m_LightColor = { 1.0f, 1.0f, 0.8f };
        float            m_LightIntensity = 20.0f;
        int m_bounces = 2.0f;

        // Cámara extras
        float m_CameraYaw = 0.0f;
        float m_CameraPitch = 0.0f;
        float m_CameraMoveSpeed = 1.0f;
        float m_CameraRotationSpeed = 0.01f;
        bool paused = false;
        bool  m_AutoZoomActive = false;
        float m_AutoZoomSpeed = 1.0f;


    };

} // namespace Diligent