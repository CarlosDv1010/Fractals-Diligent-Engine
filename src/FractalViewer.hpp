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
		void CreateIndexBuffer();

        RefCntAutoPtr<IPipelineState>         m_pPSO;
        RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
        RefCntAutoPtr<IBuffer>                m_VertexBuffer;
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
        RefCntAutoPtr<IBuffer>                m_VSConstants;
        int    m_SelectedFractal2D = 0;          // 0 = Mandelbrot por defecto
        int    m_SelectedFractal3D = 0;          // 0 = Mandelbulb por defecto
        bool   first_timeUI = true;

        // Variables de configuración del fractal
        float  m_Time = 0.0f;      // Tiempo inicial
        int    m_FractalType = 0;          // Empezamos en Mandelbrot

        // Cámara
        FirstPersonCamera m_Camera;                // cámara por defecto
        bool              m_is3D = false;

        // Transformaciones comunes
        float m_Zoom = 1.0f;          // zoom = 1 (sin zoom)
        float m_OffsetX = 0.0f, m_OffsetY = 0.0f, m_OffsetZ = 0.0f;

        // Colores y fondo
        float4 m_FractalColor = float4{ 1,1,1,1 }; // blanco
        float4 m_BackgroundColor = float4{ 0,0,0,1 }; // negro

        // Constante C (Julia)
        float4 m_FractalC = float4{ 0,0,0,0 };

		int m_maxiter = 100; // iteraciones máximas

        // Parámetros generales del fractal
		// x = bailout, y = power (no usado aquí), z = usesDoublePrecision
        float3 m_FractalParams1 = float3{ 2.0f, 2.0f, 0.0f };
        // x = gamma, yzw = libres
        float4 m_FractalParams2 = float4{ 1.0f, 0, 0, 0 };

        // Opciones de render
        // x = gammaMode(0/1), y = shadingMode(0/1), z/w sin uso
        float4 m_Options3D = float4{ 0,0,0,0 };

        // Animación
        // x = timeScale, y = speedY, z = swirlSpeed, w = seed
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