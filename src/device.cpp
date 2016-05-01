#include"device.h"

#include<vector>

namespace simpleVulkan
{
    Devices::Devices(){}
    Devices::~Devices(){}
   
    Result Devices::create(vk::Instance instance, bool validate)
    {
        vk::Result result;
        //get PhysicalDevices
        uint32_t physDevicesNum = 0;
        result = instance.enumeratePhysicalDevices(&physDevicesNum,nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
        m_physicalDevices.resize(physDevicesNum);
        result = instance.enumeratePhysicalDevices(
                &physDevicesNum,
                m_physicalDevices.data());

        if(result != vk::Result::eSuccess)
        {
            return result;
        }

        //create Devices
        m_devices.resize(m_physicalDevices.size());
        for(size_t deviceIndex=0;deviceIndex<m_devices.size();++deviceIndex)
        {

            //get QueueFamilyProperties
            uint32_t queueFamilyPropertiesCount;
            m_physicalDevices[deviceIndex].getQueueFamilyProperties(&queueFamilyPropertiesCount, nullptr);
            std::vector<vk::QueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
            m_physicalDevices[deviceIndex].getQueueFamilyProperties(&queueFamilyPropertiesCount, queueFamilyProperties.data());

            uint32_t familyIndex;
            for (int i =0; i < queueFamilyProperties.size(); ++i)
            {
                if (queueFamilyProperties[i].queueFlags() & vk::QueueFlagBits::eGraphics)
                {
                    familyIndex = i;
                    break;
                }
            }
            //init DeviceQueueInfo
            vk::DeviceQueueCreateInfo queueInfo;
            float queueProperties {0.0f};
            queueInfo.queueFamilyIndex(familyIndex);
            queueInfo.queueCount(1);
            queueInfo.pQueuePriorities(&queueProperties);

            //init DeviceCreateInfo
            vk::DeviceCreateInfo deviceInfo;
            deviceInfo.queueCreateInfoCount(1);
            deviceInfo.pQueueCreateInfos(&queueInfo);

            std::vector<const char*> layers;
            std::vector<const char*> extensions;

            extensions.push_back("VK_KHR_swapchain");
            deviceInfo.enabledExtensionCount(extensions.size());
            deviceInfo.ppEnabledExtensionNames(extensions.data());


            if (validate)
            {
                layers.push_back("VK_LAYER_LUNARG_standard_validation");
            }

            deviceInfo.enabledLayerCount(layers.size());
            deviceInfo.ppEnabledLayerNames(layers.data());

            //create Device
            result = m_physicalDevices[deviceIndex].createDevice(&deviceInfo,nullptr,&m_devices[deviceIndex]);
        }
        return result;
    }

    void Devices::destroy()
    {
        for(vk::Device& device:m_devices)
        {
           device.destroy(nullptr);
        }
    }

	vk::PhysicalDevice& Devices::getVkPhysicalDevice(size_t index)
	{
		return m_physicalDevices[index];
	}
	vk::Device& Devices::getVkDevice(size_t index)
    {
        return m_devices[index];
    }


}
