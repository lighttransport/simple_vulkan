#include"device.h"

#include<vector>

#define ENABLE_VALIDATION	(1)

namespace simpleVulkan
{
    Device::Device(){}
    Device::~Device(){}
   
    Result Device::create(vk::Instance instance)
    {
        vk::Result result;
        //get PhysicalDevices
        uint32_t physDevicesNum = 0;
        result = instance.enumeratePhysicalDevices(&physDevicesNum,nullptr);
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
        std::vector<vk::PhysicalDevice> physDevices(physDevicesNum);
        result = instance.enumeratePhysicalDevices(
                &physDevicesNum,
                physDevices.data());
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
		m_physicalDevice = physDevices[0];

		//get QueueFamilyProperties
		uint32_t queueFamilyPropertiesCount;
		m_physicalDevice.getQueueFamilyProperties(&queueFamilyPropertiesCount, nullptr);
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
		m_physicalDevice.getQueueFamilyProperties(&queueFamilyPropertiesCount, queueFamilyProperties.data());

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

#if ENABLE_VALIDATION
		std::vector<const char*> layers;
		layers.push_back("VK_LAYER_LUNARG_standard_validation");
		deviceInfo.enabledLayerCount(layers.size());
		deviceInfo.ppEnabledLayerNames(layers.data());
#endif

        //create Device
        result = m_physicalDevice.createDevice(&deviceInfo,nullptr,&m_device);
        return result;
    }

    void Device::destroy()
    {
        m_device.destroy(nullptr);
    }

	vk::PhysicalDevice& Device::getVkPhysicalDevice()
	{
		return m_physicalDevice;
	}
	vk::Device& Device::getVkDevice()
    {
        return m_device;
    }


}
