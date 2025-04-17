cmake_minimum_required(VERSION 3.10)
project(MiProyectoFractal)

add_definitions(-DPLATFORM_WIN32)

# Ruta al DiligentEngine que ya tienes descargado
set(DILIGENT_ROOT "C:/Users/carlo/Desktop/DiligentEngine")

# Para evitar warnings innecesarios
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agrega tu carpeta src
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.hpp")

list(APPEND SRC_FILES
    src/CreateSample.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE YES
)

if(WIN32)
    # Define Unicode para las API de Win32
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        UNICODE
        _UNICODE
    )

    # Indica al linker que es una aplicación de ventana (usa WinMain)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Incluye los headers de Diligent
target_include_directories(${PROJECT_NAME} PRIVATE
    ${DILIGENT_ROOT}/DiligentSamples/Tutorials/Tutorial21_RayTracing/src
    ${DILIGENT_ROOT}/DiligentTools/NativeApp/include
    ${DILIGENT_ROOT}/DiligentTools/NativeApp/include/Win32
    ${DILIGENT_ROOT}/DiligentSamples/SampleBase/include
    ${DILIGENT_ROOT}/DiligentCore/Common/interface
    ${DILIGENT_ROOT}/DiligentCore/Platforms/Win32/interface
    ${DILIGENT_ROOT}/DiligentCore/Platforms/Basic/interface
    ${DILIGENT_ROOT}/DiligentCore/Primitives/interface
    ${DILIGENT_ROOT}/DiligentCore/Platforms/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsTools/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngine/interface
    ${DILIGENT_ROOT}/DiligentTools/TextureLoader/interface
    ${DILIGENT_ROOT}/DiligentTools/ThirdParty/imgui
    ${DILIGENT_ROOT}/DiligentTools/Imgui/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsAccessories/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngineD3D11/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngineD3DBase/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngineD3D12/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngineOpenGL/interface
    ${DILIGENT_ROOT}/DiligentCore/Graphics/GraphicsEngineVulkan/interface
	${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12/Debug
)

# Linkea las libs ya compiladas
link_directories(
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Win32
)

# Aquí ajustas las librerías según el backend que uses
# Añadir las librerías al linker
target_link_libraries(${PROJECT_NAME}
    # Librerías de Diligent Tools
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/NativeApp/Debug/Diligent-NativeAppBase.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentSamples/SampleBase/Debug/Diligent-SampleBase.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/TextureLoader/Debug/Diligent-TextureLoader.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/ThirdParty/libpng/Debug/libpng16_staticd.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/ThirdParty/libtiff/Debug/LibTiff.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/ThirdParty/Debug/ZLib.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/ThirdParty/libjpeg-9e/Debug/LibJpeg.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentTools/Imgui/Debug/Diligent-Imgui.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsTools/Debug/Diligent-GraphicsTools.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/xxHash/cmake_unofficial/Debug/xxhash.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/Archiver/Debug/Diligent-Archiver-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineVulkan/Debug/Diligent-GraphicsEngineVk-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineOpenGL/Debug/Diligent-GraphicsEngineOpenGL-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glew/Debug/glew-static.lib

    # Librerías específicas de DirectX y OpenGL
    opengl32.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D11/Debug/Diligent-GraphicsEngineD3D11-static.lib
    d3d11.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12/Debug/Diligent-GraphicsEngineD3D12-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineNextGenBase/Debug/Diligent-GraphicsEngineNextGenBase.lib
    dxgi.lib
    d3dcompiler.lib

    # Librerías de SPIRV
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-glsld.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3DBase/Debug/Diligent-GraphicsEngineD3DBase.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/ShaderTools/Debug/Diligent-ShaderTools.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/HLSL2GLSLConverterLib/Debug/Diligent-HLSL2GLSLConverterLib.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-cored.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glslang/SPIRV/Debug/SPIRVd.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glslang/glslang/Debug/glslangd.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Tools/source/opt/Debug/SPIRV-Tools-opt.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Tools/source/Debug/SPIRV-Tools.lib

    # Librerías principales de Diligent Graphics Engine
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngine/Debug/Diligent-GraphicsEngine.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsAccessories/Debug/Diligent-GraphicsAccessories.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Common/Debug/Diligent-Common.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Win32/Debug/Diligent-Win32Platform.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Basic/Debug/Diligent-BasicPlatform.lib

    # Librerías estándar de Windows
    Shlwapi.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D11/Debug/GraphicsEngineD3D11_64d.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12/Debug/GraphicsEngineD3D12_64d.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineOpenGL/Debug/GraphicsEngineOpenGL_64d.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineVulkan/Debug/GraphicsEngineVk_64d.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Primitives/Debug/Diligent-Primitives.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
    advapi32.lib
)
