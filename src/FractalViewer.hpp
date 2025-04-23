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
            float4 TimeAndResolution;  // x = time, y = resolution.x, z = resolution.y, w = fractalType

            // Cámara (usada en 3D)
            float4 CameraPos;          // xyz = posición, w = is3D (0.0 o 1.0)
            float4 CameraDirX;         // xyz = right
            float4 CameraDirY;         // xyz = up
            float4 CameraDirZ;         // xyz = forward

            // Transformaciones comunes
            float4 ZoomOffset;         // x = zoom/scale, y = offset.x, z = offset.y, w = offset.z (3D)

            // Parámetros de color
            float4 FractalColor;       // rgba o como multiplicador general
            float4 BackgroundColor;    // para mezclar o dejar fondo personalizable

            // Constante C (útil para Julia y otras variantes)
            float4 FractalC;           // x = c.x, y = c.y, z/w pueden ser usados como animación extra

            // Parámetros generales del fractal
            int maxiter;            // número máximo de iteraciones
            float3 FractalParams1;     // x = maxIterations, y = bailout, z = power, w = escapeOffset
            float4 FractalParams2;     // valores adicionales si se requiere, puedes usarlo libremente

            // Opciones de render o efectos especiales
            float4 Options3D;        // x=maxSteps, y=maxDist, z=threshold, w=pause(unused)

            // Para efectos de tiempo, movimiento o animaciones
            float4 AnimationParams;    // x = velocidad X, y = velocidad Y, z = deformación, w = seed o fase
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