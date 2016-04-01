#pragma once

#include<tuple>
#include<vector>

#include<vulkan/vk_cpp.h>
#include"device.h"

namespace simpleVulkan
{
    class DescriptorSets
    {
        vk::Device m_device;
        vk::DescriptorPool m_descriptorPool;
        vk::DescriptorSetLayout m_descriptorSetLayout;
        std::vector<vk::DescriptorSet> m_descriptorSets;
   public:
        DescriptorSets();
        ~DescriptorSets();

        Result create(
            vk::Device device,
            const std::vector<vk::DescriptorSetLayoutBinding>& descriptorBindings,
            uint32_t setsCount);
        void destroy();

        vk::DescriptorSet& getVkDescriptorSet(size_t index);
        vk::DescriptorSetLayout& getVkDescriptorSetLayout();
    };
}
