# SimpleVulkan

SimpleVulkan is simple vulkan framework.  

## Dependecies

* vulkan 1.0.0+
* glfw 3.2+
* vkcpp <https://developer.nvidia.com/open-source-vulkan-c-api>

## Supported platform

* [x] Linux
* [x] Windows(Visual Studio)

## Build

### Windows

Visual Studio 2013 is required to build.

    $ ./tools/win/premake5.exe vs2013

## Run

    $ ./vulkan_test <options>

In default, Vulkan validation layer + debug report is enabled. Disable validation layer with `-novalidate` argument.

## License

`SimpleVulkan` is licensed under MIT license
