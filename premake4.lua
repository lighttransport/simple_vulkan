sources = {
   "src/*.cpp"
 , "src/application.cpp"
 , "src/buffer.cpp"
 , "src/command_buffers.cpp"
 , "src/descriptor_sets.cpp"
 , "src/device.cpp"
 , "src/frame_buffer.cpp"
 , "src/graphics_pipeline.cpp"
 , "src/image.cpp"
 , "src/instance.cpp"
 , "src/main.cpp"
 , "src/queue.cpp"
 , "src/render_pass.cpp"
 , "src/shader.cpp"
 , "src/swapchain.cpp"
}

solution "SimpleVulkanSolution"
   configurations { "Release", "Debug" }

   platforms { "x64", "x32" }

   -- A project defines one build target
   project "SimpleVulkan"
      kind "ConsoleApp"
      language "C++"

      files { sources }

      includedirs {
         "include/",
         "deps/vulkan/include",
      }

      -- Windows general
      configuration { "windows" }

         includedirs { "./deps/GLFW3/windows/include" }
         libdirs { "./deps/GLFW3/windows/lib" }

         libdirs { "./deps/vulkan/windows/lib" }

         links { "glfw3",  "vulkan-1" }

         defines { 'NOMINMAX', '_LARGEFILE_SOURCE', '_FILE_OFFSET_BITS=64' }

      -- Linux specific
      configuration {"linux", "gmake"}
         defines { '__STDC_CONSTANT_MACROS', '__STDC_LIMIT_MACROS' } -- c99
         links{"glfw3","vulkan","rt","m","dl","pthread","X11","Xrandr","Xinerama","Xxf86vm","Xcursor"}
         buildoptions {"-std=c++11"}
         linkoptions {"-std=c++11"}


      configuration "Debug"
         defines { "DEBUG" } -- -DDEBUG
         flags { "Symbols" }
         targetname "vulkan_test_debug"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Symbols", "Optimize" }
         targetname "vulkan_test"
