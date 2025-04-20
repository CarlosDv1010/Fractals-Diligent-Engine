#include "FractalViewer.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "ColorConversion.h"

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

        struct Constants
        {
            float Time;
            float2 Resolution;
            float Padding; // Para alinear a 16 bytes (float4)
        };


        Constants CBufferData = {};
        CBufferData.Time = m_Time;
        CBufferData.Resolution = float2{ static_cast<float>(m_pSwapChain->GetDesc().Width),
                                         static_cast<float>(m_pSwapChain->GetDesc().Height) };

        // Map y copiar datos
        MapHelper<Constants> CBData(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
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
    }

} // namespace Diligent