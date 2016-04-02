#include"instance.h"

namespace simpleVulkan
{
    Instance::Instance(){}
    Instance::~Instance(){}

    Result Instance::create(
            std::string applicationName,
            std::uint32_t applicationVersion,
            std::string engineName,
            std::uint32_t engineVersion,
            const std::vector<const char*>& extensions,
            const std::vector<const char*>& layers)
    {
        vk::Result result;

        //init ApplicationInfo
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName(applicationName.c_str()); 
        appInfo.applicationVersion(applicationVersion);
        appInfo.pEngineName(engineName.c_str());
        appInfo.engineVersion(engineVersion);
        appInfo.apiVersion(VK_API_VERSION);
       
        //std::vector<const char*> layers;
        //layers.push_back("VK_LAYER_LUNARG_param_checker");
        //layers.push_back("VK_LAYER_LUNARG_swapchain");
        //layers.push_back("VK_LAYER_LUNARG_mem_tracker");
        //layers.push_back("VK_LAYER_GOOGLE_unique_objects");
        //deviceInfo.enabledLayerCount(layers.size());
        //deviceInfo.ppEnabledLayerNames(layers.data());

        //init InstanceCreateInfo
        vk::InstanceCreateInfo instInfo;
        instInfo.pApplicationInfo(&appInfo);
        instInfo.enabledExtensionCount(extensions.size());
        instInfo.ppEnabledExtensionNames(extensions.data());
        instInfo.enabledLayerCount(layers.size());
        instInfo.ppEnabledLayerNames(layers.data());

        //create Instance
        result = vk::createInstance(&instInfo,nullptr,&m_instance);
      
        return result;
    }

    void Instance::destroy()
    {
        m_instance.destroy(nullptr);
    }

    vk::Instance& Instance::getVkInstance()
    {
        return m_instance;
    }

}
