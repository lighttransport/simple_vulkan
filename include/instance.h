#pragma once

#include<string>
#include<vector>
#include<vulkan/vk_cpp.h>

#include<iostream>

namespace simpleVulkan
{
    using Result = vk::Result;
    class Instance
    {
       vk::Instance m_instance;
   public:
       Instance();
       ~Instance();

       Result create(
               std::string applicationName,
               std::uint32_t applicationVersion,
               std::string engineName,
               std::uint32_t engineVersion,
               const std::vector<const char*>& extensions,
               const std::vector<const char*>& layers);
       void destroy();

       vk::Instance& getVkInstance();
    };
}
