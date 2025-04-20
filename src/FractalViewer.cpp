#include "FractalViewer.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "ColorConversion.h"
#include "imgui.h"

namespace Diligent
{
    
    void FractalViewer::CreatePipelineState()
    {
        // Pipeline state object encompasses configuration of all GPU stages

        GraphicsPipelineStateCreateInfo PSOCreateInfo;

        // Pipeline state name is used by the engine to report issues.
        // It is always a good idea to give objects descriptive names.
        PSOCreateInfo.PSODesc.Name = "Fractal PSO";

        // This is a graphics pipeline
        PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        // clang-format off
        // This tutorial will render to a single render target
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        // Set render target format which is the format of the swap chain's color buffer
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
        // Set depth buffer format which is the format of the swap chain's back buffer
        PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
        // Primitive topology defines what kind of primitives will be rendered by this pipeline state
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // Cull back faces
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
        // Enable depth testing
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
        // clang-format on

        ShaderCreateInfo ShaderCI;
        // Tell the system that the shader source code is in HLSL.
        // For OpenGL, the engine will convert this into GLSL under the hood.
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.Desc.UseCombinedTextureSamplers = true;

        // Pack matrices in row-major order
        ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;

        // Presentation engine always expects input in gamma space. Normally, pixel shader output is
        // converted from linear to gamma space by the GPU. However, some platforms (e.g. Android in GLES mode,
        // or Emscripten in WebGL mode) do not support gamma-correction. In this case the application
        // has to do the conversion manually.
        ShaderMacro Macros[] = { {"CONVERT_PS_OUTPUT_TO_GAMMA", m_ConvertPSOutputToGamma ? "1" : "0"} };
        ShaderCI.Macros = { Macros, _countof(Macros) };

        // In this tutorial, we will load shaders from file. To be able to do that,
        // we need to create a shader source stream factory
        RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
        m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);
        ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
        // Create a vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
            ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = "Quad VS";
            ShaderCI.FilePath = "../Shaders/quad.vsh";
            m_pDevice->CreateShader(ShaderCI, &pVS);
        }

        // Create a pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
            ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
            ShaderCI.EntryPoint = "main";
            ShaderCI.Desc.Name = "Fractal PS";
            ShaderCI.FilePath = "../Shaders/fractal.psh";
            m_pDevice->CreateShader(ShaderCI, &pPS);

            BufferDesc CBDesc;
            CBDesc.Name = "VS constants CB";
            CBDesc.Size = sizeof(float4);
            CBDesc.Usage = USAGE_DYNAMIC;
            CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
            CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
            m_pDevice->CreateBuffer(CBDesc, nullptr, &m_VSConstants);
        }

        // clang-format off
        // Define vertex shader input layout
        LayoutElement LayoutElems[] =
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, False}, // ATTRIB0: pos
            LayoutElement{1, 0, 2, VT_FLOAT32, False}  // ATTRIB1: uv
        };

        // clang-format on
        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        // Define variable type that will be used by default
        PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);

        m_pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_VSConstants);

        m_pPSO->CreateShaderResourceBinding(&m_pSRB, true);

    }

    void FractalViewer::CreateIndexBuffer() {
        static const Uint32 QuadIndices[] = {
            0, 1, 2,  // Primer triángulo
            2, 1, 3   // Segundo triángulo
        };
        // clang-format on

        BufferDesc IndBuffDesc;
        IndBuffDesc.Name = "Cube index buffer";
        IndBuffDesc.Usage = USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
        IndBuffDesc.Size = sizeof(QuadIndices);
        BufferData IBData;
        IBData.pData = QuadIndices;
        IBData.DataSize = sizeof(QuadIndices);
        m_pDevice->CreateBuffer(IndBuffDesc, &IBData, &m_IndexBuffer);
    }

    void FractalViewer::CreateVertexBuffer() {
        struct Vertex
        {
            float3 pos;
            float2 uv;
        };

        static const Vertex QuadVerts[] =
        {
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // inferior izquierda
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}}, // superior izquierda
            {{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // inferior derecha
            {{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}}, // superior derecha
        };


        BufferDesc VertBuffDesc;
        VertBuffDesc.Name = "Fullscreen Quad vertex buffer";
        VertBuffDesc.Usage = USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        VertBuffDesc.Size = sizeof(QuadVerts);

        BufferData VBData;
        VBData.pData = QuadVerts;
        VBData.DataSize = sizeof(QuadVerts);

        m_pDevice->CreateBuffer(VertBuffDesc, &VBData, &m_VertexBuffer);
    }


    void FractalViewer::Initialize(const SampleInitInfo& InitInfo)
    {
        SampleBase::Initialize(InitInfo);

        CreatePipelineState();
        CreateVertexBuffer();
        CreateIndexBuffer();

        m_Zoom = 1.0f;
        m_OffsetX = m_OffsetY = m_OffsetZ = 0.0f;
        m_FractalParams1 = float4(100, 2, 0, 0);
        m_FractalParams2 = float4(1, 0, 0, 0); // gamma = 1
        m_FractalColor = float4(1, 1, 1, 1);
        m_BackgroundColor = float4(0, 0, 0, 1);
        m_RenderFlags = float4(0, 0, 0, 0); // sin gamma, sin shading

    }

    // Render a frame
    void FractalViewer::Render()
    {
        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        // Clear the back buffer
        float4 ClearColor = { 0.350f, 0.350f, 0.350f, 1.0f };
        if (m_ConvertPSOutputToGamma)
        {
            // If manual gamma correction is required, we need to clear the render target with sRGB color
            ClearColor = LinearToSRGB(ClearColor);
        }
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        struct ShaderConstants
        {
            float4 TimeAndResolution;  // x = time, y = resolution.x, z = resolution.y, w = fractalType

            // Cámara (usada tanto en 2D como en 3D)
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
            float4 RenderFlags;        // x = colorMode, y = shadingMode, z = useDistanceEstimation, w = debugView

            // Para efectos de tiempo, movimiento o animaciones
            float4 AnimationParams;    // x = velocidad X, y = velocidad Y, z = deformación, w = seed o fase
        };

        ShaderConstants CBufferData = {};

        // 1. Tiempo, resolución y tipo de fractal
        float fracType = m_is3D
            ? static_cast<float>(m_SelectedFractal3D)
            : static_cast<float>(m_SelectedFractal2D);
        CBufferData.TimeAndResolution = float4{
            m_Time,
            static_cast<float>(m_pSwapChain->GetDesc().Width),
            static_cast<float>(m_pSwapChain->GetDesc().Height),
            fracType
        };

        // 2. Cámara
        const auto& pos = m_Camera.GetPos();
        const auto& right = m_Camera.GetWorldRight();
        const auto& up = m_Camera.GetWorldUp();
        const auto& forward = m_Camera.GetWorldAhead();
        CBufferData.CameraPos = float4{ pos,  m_is3D ? 1.0f : 0.0f };
        CBufferData.CameraDirX = float4{ right,   0.0f };
        CBufferData.CameraDirY = float4{ up,      0.0f };
        CBufferData.CameraDirZ = float4{ forward, 0.0f };

        // 3. Zoom y offset
        CBufferData.ZoomOffset = float4{
            m_Zoom,
            m_OffsetX,
            m_OffsetY,
            m_is3D ? m_OffsetZ : 0.0f
        };

        // 4. Color y fondo
        CBufferData.FractalColor = m_FractalColor;
        CBufferData.BackgroundColor = m_BackgroundColor;

        // 5. Constante C (Julia)
        CBufferData.FractalC = m_FractalC;

		CBufferData.maxiter = m_maxiter; // número máximo de iteraciones

        // 6. Parámetros del fractal
        CBufferData.FractalParams1 = m_FractalParams1;
        CBufferData.FractalParams2 = m_FractalParams2;

        // 7. Flags de render
        CBufferData.RenderFlags = m_RenderFlags;

        // 8. Animación
        CBufferData.AnimationParams = m_AnimationParams;

        // 9. Mapear y enviar a GPU
        MapHelper<ShaderConstants> CBData{ m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD };
        *CBData = CBufferData;

        IBuffer* pVBs[] = { m_VertexBuffer };
        Uint64 Offsets[] = { 0 };
        m_pImmediateContext->SetVertexBuffers(0, 1, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
		// Set index buffer
		m_pImmediateContext->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


        // Set the pipeline state
        m_pImmediateContext->SetPipelineState(m_pPSO);
        // Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode
        // makes sure that resources are transitioned to required states.
        m_pImmediateContext->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs drawAttrs;
        drawAttrs.IndexType = VT_UINT32;
        drawAttrs.NumIndices = 6;
        drawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
        m_pImmediateContext->DrawIndexed(drawAttrs);

    }

    void FractalViewer::Update(double CurrTime, double ElapsedTime, bool DoUpdateUI)
    {
        SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);
        m_Time += static_cast<float>(ElapsedTime); 
        if (m_is3D)
            m_Camera.Update(m_InputController, static_cast<float>(ElapsedTime));
    }


    void FractalViewer::UpdateUI()
    {
        if (first_timeUI)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
            first_timeUI = false;
        }

        if (ImGui::Begin("Fractal Settings"))
        {
            // --- Fractal type selection (2D / 3D) ---
            ImGui::Separator();
            static int current2D = 0, current3D = 0;
            const char* fractal2DOptions[] = { "Mandelbrot","Julia","Burning Ship","Custom 2D" };
            const char* fractal3DOptions[] = { "Mandelbulb","Menger Sponge","Kaleidoscopic IFS","Custom 3D" };
            if (ImGui::CollapsingHeader("2D Fractals", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Combo("2D Type", &current2D, fractal2DOptions, IM_ARRAYSIZE(fractal2DOptions));
            }
            if (ImGui::CollapsingHeader("3D Fractals", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Combo("3D Type", &current3D, fractal3DOptions, IM_ARRAYSIZE(fractal3DOptions));
            }
            m_SelectedFractal2D = current2D;
            m_SelectedFractal3D = current3D;

            // Switch mode
            ImGui::Separator();
            ImGui::Checkbox("3D MODE", &m_is3D);

            // --- Cámara ---
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Position
                float3 pos = m_Camera.GetPos();
                if (ImGui::InputFloat3("Position", &pos.x))
                    m_Camera.SetPos(pos);

                // Rotation
                ImGui::SliderFloat("Yaw", &m_CameraYaw, -180.0f, 180.0f);
                ImGui::SliderFloat("Pitch", &m_CameraPitch, -89.0f, 89.0f);
                m_Camera.SetRotation(m_CameraYaw, m_CameraPitch);

                // Projection attributes
                auto proj = m_Camera.GetProjAttribs();
                float nearP = proj.NearClipPlane;
                float farP = proj.FarClipPlane;
                float aspect = proj.AspectRatio;
                float fov = proj.FOV;
                if (ImGui::InputFloat("Near Clip", &nearP) ||
                    ImGui::InputFloat("Far Clip", &farP) ||
                    ImGui::InputFloat("Aspect", &aspect) ||
                    ImGui::InputFloat("FOV", &fov))
                {
                    m_Camera.SetProjAttribs(nearP, farP, aspect, fov,
                        proj.PreTransform, proj.IsGL);
                }

                // Speeds
                if (ImGui::SliderFloat("Move Speed", &m_CameraMoveSpeed, 0.01f, 100.f))
                    m_Camera.SetMoveSpeed(m_CameraMoveSpeed);
                if (ImGui::SliderFloat("Rotation Speed", &m_CameraRotationSpeed, 0.001f, 1.0f))
                    m_Camera.SetRotationSpeed(m_CameraRotationSpeed);
            }

            // --- Transformaciones comunes ---
            ImGui::Separator();
            ImGui::Text("Transformations:");
            ImGui::DragFloat("Zoom", &m_Zoom, 0.2f, 0.001f, 1000000.0f, "%.6f");
            ImGui::DragFloat("Offset X", &m_OffsetX, 0.0005f, -2.0f, 2.0f, "%.6f");
            ImGui::DragFloat("Offset Y", &m_OffsetY, 0.0005f, -2.0f, 2.0f, "%.6f");
            if (m_is3D)
                ImGui::DragFloat("Offset Z", &m_OffsetZ, 0.001f, -2.0f, 2.0f, "%.6f");


            // --- Colores ---
            ImGui::Separator();
            ImGui::Text("Colors:");
            ImGui::ColorEdit4("Fractal Color", &m_FractalColor.x);
            ImGui::ColorEdit4("Background Color", &m_BackgroundColor.x);

            // --- Constante C para Julia ---
            ImGui::Separator();
            ImGui::Text("C Constant (Julia):");
            ImGui::InputFloat2("C (x, y)", &m_FractalC.x);

            // --- Parámetros del fractal ---
            ImGui::Separator();
            ImGui::Text("Fractal Parameters:");
            ImGui::SliderInt("Max Iter", (int*)&m_maxiter, 10, 10000);
            ImGui::SliderFloat("Bailout", &m_FractalParams1.y, 1.0f, 10.0f);
            ImGui::SliderFloat("Power", &m_FractalParams1.z, 1.0f, 10.0f);
            ImGui::SliderFloat("Escape Offs", &m_FractalParams1.w, 0.0f, 5.0f);

            // --- Flags de render ---
            ImGui::Separator();
            ImGui::Text("Render Options:");
            ImGui::Checkbox("Color Mode", (bool*)&m_RenderFlags.x);
            ImGui::Checkbox("Shading Mode", (bool*)&m_RenderFlags.y);
            ImGui::Checkbox("Use Distance Estimation", (bool*)&m_RenderFlags.z);
            ImGui::Checkbox("Debug View", (bool*)&m_RenderFlags.w);
            ImGui::SliderFloat("Gamma", &m_FractalParams2.x, 0.1f, 5.0f);


            // --- Animación ---
            ImGui::Separator();
            ImGui::Text("Animation Params:");
            ImGui::SliderFloat("Speed X", &m_AnimationParams.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Speed Y", &m_AnimationParams.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Deformation", &m_AnimationParams.z, -2.0f, 2.0f);
            ImGui::SliderFloat("Phase/Seed", &m_AnimationParams.w, 0.0f, 1.0f);

            ImGui::End();
        }
    }






} // namespace Diligent