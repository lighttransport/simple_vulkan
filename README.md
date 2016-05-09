# SimpleVulkan

SimpleVulkan is simple vulkan framework.  

## Dependecies

* LunarG Vulkan SDK 1.0.11+
* glfw 3.2+
  * Precompiled binary is privided in `deps/windows/glfw3/` for Windows platform.
* vkcpp <https://developer.nvidia.com/open-source-vulkan-c-api>

## Supported platform

* [x] Linux
* [x] Windows 64bit
  * Visual Studio 2013 + x64

## Build

### Windows

First checkout dependent libraries(e.g. vkcpp).

    $ git submodule update --init

Set `VULKAN_SDK` environment variable to the path to Vulkan SDK.

Build glfw3 library.

Then,

    $ ./tools/win/premake5.exe vs2013

Build .sln with Visual Studio 2013.
(Only 64bit build is supported).

## Run

Compile `shaders/triangle.frag` and `shaders/triangle.vert` with `glslangValidator -V`, and put `frag.spv` and `vert.spv` to working directory.

Then,

    $ ./vulkan_test <options>

In default, Vulkan validation layer + debug report is enabled. Disable validation layer with `-novalidate` argument.

## License

`SimpleVulkan` is licensed under MIT license
