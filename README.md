# Fractal Rendering Engine

Explorador de fractales en tiempo real programado en C++ usando Diligent Engine. Implementa renderizado de fractales 2D y 3D mediante Raymarching y Compute Shaders.

## Funcionalidades
- Fractales 2D: Conjunto de Mandelbrot y Burning Ship.
- Fractales 3D: Mandelbulb y Esponja de Menger.
- Navegación interactiva de cámara y ajuste de parámetros en tiempo real.
- Aceleración por hardware mediante HLSL Compute Shaders.
- Soporte para DirectX 11/12, Vulkan y OpenGL.

## Resultados
[Link al vídeo de YouTube](https://www.youtube.com/watch?v=NoZW9JOzLVc)

## Detalles técnicos
Usa Raymarching con estimadores de distancia (Distance Estimation) para los fractales 3D. El cálculo se paraleliza en Compute Shaders para mantener el rendimiento. Para el Mandelbulb se implementaron estimadores basados en potencias variables y para la Menger se usa escalado recursivo.

## Requisitos y ejecución
Funciona con Diligent Engine en el commit [d9e4577](https://github.com/DiligentGraphics/DiligentEngine/commit/d9e457700ab5142776199cadcc926835c22a93f9).

### Construcción
1. Tener Diligent Engine compilado localmente en el commit mencionado.
2. En el `CMakeLists.txt`, poner la ruta de tu instalación en `DILIGENT_ROOT` o pasarla por consola: `cmake -DDILIGENT_ROOT="C:/Ruta/A/Diligent" ..`
3. Crear carpeta `build`, entrar y ejecutar `cmake ..`
4. Abrir el `.sln` en Visual Studio, dar clic derecho al proyecto **ALL_BUILD** y darle a **Build**.

### Configuración de DLLs (Crítico)
El ejecutable no abrirá si no tiene los DLLs del motor en su carpeta. Debes copiarlos manualmente desde tu build de Diligent a la carpeta de salida del proyecto (ej. `build/Debug`).

Los archivos necesarios suelen estar en rutas como:
`C:/Users/carlo/Desktop/DiligentEngine/build-Release/DiligentSamples/Tutorials/Tutorial27_PostProcessing/Release` (o similar en Debug).

Copia todos los archivos `.dll` (como `GraphicsEngineD3D12_64d.dll`, `GraphicsEngineVk_64d.dll`, etc.) al mismo nivel que el `.exe` generado.

## Créditos
- Diligent Engine por el framework gráfico.
- Inigo Quilez por la documentación sobre Distance Estimation.
