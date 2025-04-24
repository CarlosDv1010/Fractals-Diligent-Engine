#include "FractalViewer.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "ColorConversion.h"
#include "imgui.h"

namespace Diligent
{
    
    void FractalViewer::CreatePipelineState()
    {
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PSOCreateInfo.PSODesc.Name = "Fractal PSO";

        PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		ShaderCI.HLSLVersion = { 6, 3 };

        ShaderCI.Desc.UseCombinedTextureSamplers = true;

        ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;

        ShaderMacro Macros[] = { {"CONVERT_PS_OUTPUT_TO_GAMMA", m_ConvertPSOutputToGamma ? "1" : "0"} };
        ShaderCI.Macros = { Macros, _countof(Macros) };

        m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &m_pShaderSourceFactory);
        ShaderCI.pShaderSourceStreamFactory = m_pShaderSourceFactory;
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
            CBDesc.Size = sizeof(ShaderConstants);
            CBDesc.Usage = USAGE_DYNAMIC;
            CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
            CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
            m_pDevice->CreateBuffer(CBDesc, nullptr, &m_VSConstants);
        }

        // Define vertex shader input layout
        LayoutElement LayoutElems[] =
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, False}, // ATTRIB0: pos
            LayoutElement{1, 0, 2, VT_FLOAT32, False}  // ATTRIB1: uv
        };

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

    void FractalViewer::CreateComputePipelineState()
    {
        // 1. Descripción básica
        ComputePipelineStateCreateInfo PSOCreateInfo;
        PSOCreateInfo.PSODesc.Name = "Fractal Compute PSO";
        PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;

        // 2. Compilar el compute shader
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.HLSLVersion = { 6, 3 };
        ShaderCI.Desc.UseCombinedTextureSamplers = true;
        ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;
        ShaderCI.pShaderSourceStreamFactory = m_pShaderSourceFactory; // ya creado

        RefCntAutoPtr<IShader> pCS;
        {
            ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
            ShaderCI.EntryPoint = "CSMain";
            ShaderCI.Desc.Name = "Fractal CS";
            ShaderCI.FilePath = "../Shaders/fractalCompute.psh";
            m_pDevice->CreateShader(ShaderCI, &pCS);

            BufferDesc CBDesc;
            CBDesc.Name = "CS Constants";
            CBDesc.Size = sizeof(ShaderConstants);
            CBDesc.Usage = USAGE_DYNAMIC;
            CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
            CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
            m_pDevice->CreateBuffer(CBDesc, nullptr, &m_VSConstantsComputeShader);
        }

        // 3. Asignar el compute shader al PSO
        PSOCreateInfo.pCS = pCS;

        // 4. Layout de recursos (same as graphics, si usas mismo CBuffer/UAV/SRV)
        PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType =
            SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        TextureDesc TexDesc;
        TexDesc.Name = "Compute Output Texture";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.Width = m_pSwapChain->GetDesc().Width;
        TexDesc.Height = m_pSwapChain->GetDesc().Height;
        TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
        TexDesc.Usage = USAGE_DEFAULT;
        TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;

        ShaderResourceVariableDesc Vars[] =
        {
            {SHADER_TYPE_COMPUTE, "OutputTex", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };

        PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
        PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        // 5. Crear el PSO
        m_pDevice->CreateComputePipelineState(PSOCreateInfo, &m_pComputePSO);

        // Crea la textura usando el dispositivo
        m_pDevice->CreateTexture(TexDesc, nullptr, &m_pComputeOutputTex);

        m_pComputePSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_VSConstantsComputeShader);

        

        m_pComputePSO->CreateShaderResourceBinding(&m_pComputeSRB, true);
        m_pComputeSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "OutputTex")
            ->Set(m_pComputeOutputTex->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS));
    }

    void FractalViewer::CreateQuadPipelineState()
    {
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PSOCreateInfo.PSODesc.Name = "Quad PSO";
        PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
        auto SCDesc = m_pSwapChain->GetDesc();
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = SCDesc.ColorBufferFormat;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = SCDesc.DepthBufferFormat;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

        LayoutElement QuadLayout[] = {
            LayoutElement{0, 0, 3, VT_FLOAT32, False},
            LayoutElement{1, 0, 2, VT_FLOAT32, False}
        };

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = QuadLayout;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(QuadLayout);

        RefCntAutoPtr<IShader> pVS, pPS;
        ShaderCreateInfo SC;
        SC.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        SC.HLSLVersion = { 6, 3 };
        SC.pShaderSourceStreamFactory = m_pShaderSourceFactory;
        SC.CompileFlags = SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR;

        SC.Desc.UseCombinedTextureSamplers = true;
        SC.Desc.ShaderType = SHADER_TYPE_VERTEX;
        SC.EntryPoint = "main";
        SC.Desc.Name = "Quad VS";
        SC.FilePath = "../Shaders/quad.vsh";

        m_pDevice->CreateShader(SC, &pVS);

        SC.Desc.ShaderType = SHADER_TYPE_PIXEL;
        SC.EntryPoint = "main";
        SC.Desc.Name = "Quad PS";
        SC.FilePath = "../Shaders/quad_ps.psh";

        m_pDevice->CreateShader(SC, &pPS);
        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        ShaderResourceVariableDesc Vars[] =
        {
            {SHADER_TYPE_PIXEL, "InputTex", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };

        SamplerDesc SamLinearClampDesc
        {
            FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
            TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };
        ImmutableSamplerDesc ImtblSamplers[] =
        {
            {SHADER_TYPE_PIXEL, "InputTex", SamLinearClampDesc}
        };

        PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
        PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);
        PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
        PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);
        PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pQuadPSO);
        m_pQuadPSO->CreateShaderResourceBinding(&m_pQuadSRB, true);
        m_pQuadSRB->GetVariableByName(SHADER_TYPE_PIXEL, "InputTex")->Set(m_pComputeOutputTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
    }

    void FractalViewer::CreateIndexBuffer() {
        static const Uint32 QuadIndices[] = {
            0, 1, 2,  // Primer triángulo
            2, 1, 3   // Segundo triángulo
        };

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
        CreateComputePipelineState();
        CreateQuadPipelineState();
        CreateVertexBuffer();
        CreateIndexBuffer();

        m_Zoom = 1.0f;
		m_Camera.SetPos({ 0.0f, 0.0f, -4.0f });
        m_FractalParams1 = float4(100, 2, 0, 0);
        m_FractalParams2 = float4(1, 0, 0, 0);
        m_FractalColor = float4(1, 1, 1, 1);
        m_BackgroundColor = float4(0, 0, 0, 1);
        m_Options3D = float4(100, 10.0f, 0.001, 0);

    }

    void FractalViewer::Render()
    {
        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        float4 ClearColor = { 0.350f, 0.350f, 0.350f, 1.0f };
        if (m_ConvertPSOutputToGamma)
        {
            ClearColor = LinearToSRGB(ClearColor);
        }
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        ShaderConstants CBufferData = {};

		// Definición de variables de shader
        {
            float fracType = m_is3D
                ? static_cast<float>(m_SelectedFractal3D)
                : static_cast<float>(m_SelectedFractal2D);
            CBufferData.TimeAndResolution = float4{
                m_Time,
                static_cast<float>(m_pSwapChain->GetDesc().Width),
                static_cast<float>(m_pSwapChain->GetDesc().Height),
                fracType
            };

            const auto& pos = m_Camera.GetPos();
            const auto& right = m_Camera.GetWorldRight();
            const auto& up = m_Camera.GetWorldUp();
            const auto& forward = m_Camera.GetWorldAhead();
            CBufferData.CameraPos = float4{ pos,  m_is3D ? 1.0f : 0.0f };
            CBufferData.CameraDirX = float4{ right,   0.0f };
            CBufferData.CameraDirY = float4{ up,      0.0f };
            CBufferData.CameraDirZ = float4{ forward, 0.0f };

            CBufferData.ZoomOffset = float4{
                m_Zoom,
                m_OffsetX,
                m_OffsetY,
                m_is3D ? m_OffsetZ : 0.0f
            };

            CBufferData.FractalColor = m_FractalColor;
            CBufferData.BackgroundColor = m_BackgroundColor;

            CBufferData.FractalC = m_FractalC;

		    CBufferData.maxiter = m_maxiter;
            CBufferData.FractalParams1 = m_FractalParams1;
            CBufferData.FractalParams2 = m_FractalParams2;

            CBufferData.Options3D = m_Options3D;
		    CBufferData.AnimationParams = m_AnimationParams;
		}

        {
            MapHelper<ShaderConstants> CBDataHelper{ m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD };
            *CBDataHelper = CBufferData;
        }
        {
            MapHelper<ShaderConstants> CBDataHelper{ m_pImmediateContext, m_VSConstantsComputeShader, MAP_WRITE, MAP_FLAG_DISCARD };
            *CBDataHelper = CBufferData;
        }

        IBuffer* pVBs[] = { m_VertexBuffer };
        Uint64 Offsets[] = { 0 };
        m_pImmediateContext->SetVertexBuffers(0, 1, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        if (m_RenderMode == RenderMode::PixelShader)
        {
            m_pImmediateContext->SetPipelineState(m_pPSO);
            m_pImmediateContext->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
            DrawIndexedAttribs attrs;
            attrs.IndexType = VT_UINT32;
            attrs.NumIndices = 6;
            attrs.Flags = DRAW_FLAG_VERIFY_ALL;
            m_pImmediateContext->DrawIndexed(attrs);
        }
        else if (m_RenderMode == RenderMode::ComputeShader) // ComputeShader
        {
            // ——— 1) Ejecutar compute shader ———
            m_pImmediateContext->SetPipelineState(m_pComputePSO);
            m_pImmediateContext->CommitShaderResources(m_pComputeSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            // Dispatch: agrupa tus hilos (aquí 16×16)
            const auto& SCDesc = m_pSwapChain->GetDesc();
            Uint32 wgX = (SCDesc.Width + 15) / 16;
            Uint32 wgY = (SCDesc.Height + 15) / 16;

            DispatchComputeAttribs DispatchAttrs;
            DispatchAttrs.ThreadGroupCountX = wgX;
            DispatchAttrs.ThreadGroupCountY = wgY;
            DispatchAttrs.ThreadGroupCountZ = 1;

            m_pImmediateContext->DispatchCompute(DispatchAttrs);

            StateTransitionDesc Barrier(
                m_pComputeOutputTex,
                RESOURCE_STATE_UNORDERED_ACCESS,
                RESOURCE_STATE_SHADER_RESOURCE,
                STATE_TRANSITION_FLAG_UPDATE_STATE
            );


            m_pImmediateContext->TransitionResourceStates(1, &Barrier);

            // ——— 2) Dibujar fullscreen-quad con la textura resultante ———
            m_pImmediateContext->SetPipelineState(m_pQuadPSO);
            // Quad SRB debería tener el SRV de la textura:
            m_pQuadSRB->GetVariableByName(SHADER_TYPE_PIXEL, "InputTex")
                ->Set(m_pComputeOutputTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
            m_pImmediateContext->CommitShaderResources(m_pQuadSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            DrawIndexedAttribs attrs;
            attrs.IndexType = VT_UINT32;
            attrs.NumIndices = 6;
            attrs.Flags = DRAW_FLAG_VERIFY_ALL;
            m_pImmediateContext->DrawIndexed(attrs);
        }

    }

    void FractalViewer::Update(double CurrTime, double ElapsedTime, bool DoUpdateUI)
    {
        SampleBase::Update(CurrTime, ElapsedTime, DoUpdateUI);

        float dt = static_cast<float>(ElapsedTime);

        if (!paused)
            m_Time += dt;
        
        if (m_is3D)
            m_Camera.Update(m_InputController, dt);

        if (m_usesComputePipeline && m_is3D) {
			m_RenderMode = RenderMode::ComputeShader;
		}
		else {
            m_RenderMode = RenderMode::PixelShader;
		}
            

        if (m_AutoZoomActive)
        {
            m_Zoom *= 1.0f + m_AutoZoomSpeed * dt;
        }
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
            const char* fractal2DOptions[] = { "Mandelbrot","Mandelbrot (Colors)","Burning Ship","Burning Ship (Colors)", "Phoenix (Colors)"};
            const char* fractal3DOptions[] = { "Mandelbulb","Menger Sponge","Quaternion Julia Set","Mandelbox" };
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
            ImGui::Checkbox("Uses Compute Pipeline", &m_usesComputePipeline);

            // --- Cámara ---
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen) && m_is3D)
            {                
                if (ImGui::SliderFloat("Move Speed", &m_CameraMoveSpeed, 0.01f, 100.f))
                    m_Camera.SetMoveSpeed(m_CameraMoveSpeed);
                if (ImGui::SliderFloat("Rotation Speed", &m_CameraRotationSpeed, 0.001f, 1.0f))
                    m_Camera.SetRotationSpeed(m_CameraRotationSpeed);
            }

            if (!m_is3D) {
                ImGui::Separator();
                ImGui::Text("Transformations:");
                ImGui::DragFloat("Zoom", &m_Zoom, 0.2f, 0.001f, 1000000.0f, "%.6f");
                ImGui::DragFloat("Offset X", &m_OffsetX, 0.0005f, -2.0f, 2.0f, "%.6f");
                ImGui::DragFloat("Offset Y", &m_OffsetY, 0.0005f, -2.0f, 2.0f, "%.6f");
                // — Botón de auto‑zoom — 
                if (ImGui::Button(m_AutoZoomActive ? "Stop Auto Zoom" : "Start Auto Zoom"))
                    m_AutoZoomActive = !m_AutoZoomActive;
                ImGui::SameLine();
                ImGui::SliderFloat("Zoom Speed", &m_AutoZoomSpeed, 0.1f, 10.0f, "%.2f");
            }
            
            ImGuiIO& io = ImGui::GetIO();
            if (!io.WantCaptureMouse && ImGui::IsMouseClicked(0))
            {
                ImGuiViewport* vp = ImGui::GetMainViewport();
                ImVec2  mpos = io.MousePos;
                ImVec2  origin = vp->Pos;      // esquina superior‑izquierda
                ImVec2  size = vp->Size;     // anchura, altura

                // 1) Normalizamos a [0,1]
                float sx = (mpos.x - origin.x) / size.x;
                float sy = (mpos.y - origin.y) / size.y;

                // 2) Convertimos a NDC [-1,1]
                float ndcX = sx * 2.0f - 1.0f;
                float ndcY = sy * 2.0f - 1.0f;  

                // 3) Ajustamos el aspect ratio
                float aspect = size.x / size.y;
                ndcX *= aspect;

                // 4) Calculo del world‑space point:
                //    uvF = ndc;   world = uvF/zoom + offset
                float worldX = ndcX / m_Zoom + m_OffsetX;
                float worldY = ndcY / m_Zoom + m_OffsetY;

                // 5) **Asignamos** el nuevo offset (no sumamos incrementalmente)
                m_OffsetX = worldX;
                m_OffsetY = worldY;
            }

            // --- Colores ---
            ImGui::Separator();
            ImGui::Text("Colors:");
            ImGui::ColorEdit4("Fractal Color", &m_FractalColor.x);
            ImGui::ColorEdit4("Background Color", &m_BackgroundColor.x);

            

            if (!m_is3D) {
                // --- Constante C para Julia ---
                ImGui::Separator();
                ImGui::Text("C Constant (Julia):");
                ImGui::InputFloat2("C (x, y)", &m_FractalC.x);

                ImGui::Separator();
                ImGui::Text("Fractal Parameters:");
                ImGui::SliderInt("Max Iter", (int*)&m_maxiter, 10, 10000);
                ImGui::SliderFloat("Bailout", &m_FractalParams1.x, 1.0f, 10.0f);
                bool tempBool = (m_FractalParams1.z > 0.5f);
                if (ImGui::Checkbox("Use Double Precision", &tempBool)) {
                    m_FractalParams1.z = tempBool ? 1.0f : 0.0f;
                }

                
            }
            
            // --- Flags de render ---
            if (m_is3D) {
                ImGui::Separator();
                ImGui::Text("Render Options:");
                ImGui::SliderFloat("Max Steps", &m_Options3D.x, 1.0f, 1000.0f);
                ImGui::SliderFloat("Max Dist", &m_Options3D.y, 0.001f, 100.0f);
                ImGui::SliderFloat("Threshold", &m_Options3D.z, 0.00001f, 0.1f, "%.5f", ImGuiSliderFlags_None);
            }

            // --- Animación ---
            ImGui::Separator();
            ImGui::Text("Animation Params:");
            ImGui::SliderFloat("Speed X", &m_AnimationParams.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Speed Y", &m_AnimationParams.y, -1.0f, 1.0f);
            ImGui::SliderFloat("Deformation", &m_AnimationParams.z, -2.0f, 2.0f);
            ImGui::SliderFloat("Phase/Seed", &m_AnimationParams.w, 0.0f, 1.0f);

			ImGui::Separator();
            ImGui::Checkbox("Paused", &paused);
            ImGui::SliderFloat("Power", &m_FractalParams1.y, 1.0f, 10.0f);
            ImGui::SliderFloat("Gamma", &m_FractalParams2.x, 0.1f, 5.0f);
            ImGui::SliderFloat("Gamma", &m_FractalParams2.y, 0.1f, 5.0f);


            ImGui::End();
        }
    }






} // namespace Diligent