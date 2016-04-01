#include"device.h"

#include<vector>

namespace simpleVulkan
{
    Device::Device(){}
    Device::~Device(){}
   
    Result Device::create(vk::Instance instance)
    {
        vk::Result result;
        //get PhysicalDevices
        uint32_t physDevicesNum;
        result = instance.enumeratePhysicalDevices(&physDevicesNum,nullptr);
        std::vector<vk::PhysicalDevice> physDevices;
        physDevices.resize(physDevicesNum);
        result = instance.enumeratePhysicalDevices(
                &physDevicesNum,
                physDevices.data());
        if(result != vk::Result::eSuccess)
        {
            return result;
        }
        //init DeviceQueueInfo
        vk::DeviceQueueCreateInfo queueInfo;
        float queueProperties {0.0f};
        queueInfo.queueFamilyIndex(0);
        queueInfo.queueCount(1);
        queueInfo.pQueuePriorities(&queueProperties);

        //init DeviceCreateInfo
        vk::DeviceCreateInfo deviceInfo;
        deviceInfo.queueCreateInfoCount(1);
        deviceInfo.pQueueCreateInfos(&queueInfo);

        //create Device
        result =  physDevices[0].createDevice(&deviceInfo,nullptr,&m_device);
        return result;
    }

    void Device::destroy()
    {
        m_device.destroy(nullptr);
    }

    vk::Device& Device::getVkDevice()
    {
        return m_device;
    }


}
