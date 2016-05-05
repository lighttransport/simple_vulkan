#include"instance.h"

#ifndef VK_API_VERSION
#define VK_API_VERSION VK_API_VERSION_1_0
#endif


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
            const std::vector<const char*>& layers,
			GLFWwindow* window)
    {
		vk::Result result;
        //init ApplicationInfo
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName(applicationName.c_str()); 
        appInfo.applicationVersion(1);
        appInfo.pEngineName("SimpleVulkan");
        appInfo.engineVersion(engineVersion);
        appInfo.apiVersion(VK_API_VERSION);

        //init InstanceCreateInfo
        vk::InstanceCreateInfo instInfo;
        instInfo.pApplicationInfo(&appInfo);
        instInfo.enabledExtensionCount(extensions.size());
        instInfo.ppEnabledExtensionNames(extensions.data());
        instInfo.enabledLayerCount(layers.size());
        instInfo.ppEnabledLayerNames(layers.data());

        //create Instance
        result = vk::createInstance(&instInfo,nullptr,&m_instance);

		//get WindowSurface
		glfwCreateWindowSurface(static_cast<VkInstance>(m_instance),window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface));

      
        return result;
    }

    void Instance::destroy()
    {
		m_instance.destroySurfaceKHR(m_surface, nullptr);
        m_instance.destroy(nullptr);
    }

    vk::Instance Instance::getVkInstance()
    {
        return m_instance;
    }

	vk::SurfaceKHR Instance::getSurface()
	{
		return m_surface;
	}

}
