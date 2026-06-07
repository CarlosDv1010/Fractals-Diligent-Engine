cmake_minimum_required(VERSION 3.10)
project(FractalRenderingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DPLATFORM_WIN32)

# Configuration: Path to Diligent Engine root
set(DILIGENT_ROOT "C:/Users/carlo/Desktop/DiligentEngine" CACHE PATH "Path to the Diligent Engine directory")

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.hpp")
file(GLOB_RECURSE SHADER_FILES
    "${CMAKE_SOURCE_DIR}/src/Shaders/*.vsh"
    "${CMAKE_SOURCE_DIR}/src/Shaders/*.psh"
    "${CMAKE_SOURCE_DIR}/src/Shaders/*.comp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${SHADER_FILES})

# IDE Folder organization
source_group(TREE "${CMAKE_SOURCE_DIR}/src/Shaders" PREFIX "Shaders" FILES ${SHADER_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    WIN32_EXECUTABLE YES
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# Shader copying logic
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/Shaders")
add_custom_target(CopyShaders ALL COMMENT "Copying shader files to output directory")

foreach(_SHADER_FILE IN LISTS SHADER_FILES)
    get_filename_component(_SHADER_NAME ${_SHADER_FILE} NAME)
    add_custom_command(
        TARGET CopyShaders PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADERS_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_SHADER_FILE}" "${SHADERS_DEST_DIR}/${_SHADER_NAME}"
        COMMENT "Copying shader ${_SHADER_NAME}"
    )
endforeach()

# DLL Copying logic (Post-build)
# Note: Adjust these paths if your Diligent build uses a different structure
set(DILIGENT_DLL_DIR "${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12/Debug") # Example source
# You can add more copy commands here for specific DLLs if needed during development


# Include directories
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

# Link directories and Libraries
link_directories(
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Win32
)

target_link_libraries(${PROJECT_NAME}
    # Diligent Tools & Samples
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
    
    # Graphics Engines
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineVulkan/Debug/Diligent-GraphicsEngineVk-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineOpenGL/Debug/Diligent-GraphicsEngineOpenGL-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D11/Debug/Diligent-GraphicsEngineD3D11-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3D12/Debug/Diligent-GraphicsEngineD3D12-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineD3DBase/Debug/Diligent-GraphicsEngineD3DBase.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngineNextGenBase/Debug/Diligent-GraphicsEngineNextGenBase.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsEngine/Debug/Diligent-GraphicsEngine.lib
    
    # Core & Third Party
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glew/Debug/glew-static.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-glsld.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/ShaderTools/Debug/Diligent-ShaderTools.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/HLSL2GLSLConverterLib/Debug/Diligent-HLSL2GLSLConverterLib.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-cored.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glslang/SPIRV/Debug/SPIRVd.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/glslang/glslang/Debug/glslangd.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Tools/source/opt/Debug/SPIRV-Tools-opt.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/ThirdParty/SPIRV-Tools/source/Debug/SPIRV-Tools.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Graphics/GraphicsAccessories/Debug/Diligent-GraphicsAccessories.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Common/Debug/Diligent-Common.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Win32/Debug/Diligent-Win32Platform.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Platforms/Basic/Debug/Diligent-BasicPlatform.lib
    ${DILIGENT_ROOT}/build/Win64/DiligentCore/Primitives/Debug/Diligent-Primitives.lib

    # System Libraries
    opengl32.lib
    d3d11.lib
    dxgi.lib
    d3dcompiler.lib
    Shlwapi.lib
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

