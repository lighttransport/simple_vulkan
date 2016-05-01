#include"application.h"
#include<vulkan/vk_cpp.h>
#include<iostream>
#include<thread>

#define ENABLE_VALIDATION	(1)

#if ENABLE_VALIDATION
    VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
            VkDebugReportFlagsEXT       flags,
            VkDebugReportObjectTypeEXT  objectType,
            uint64_t                    object,
            size_t                      location,
            int32_t                     messageCode,
            const char*                 pLayerPrefix,
            const char*                 pMessage,
            void*                       pUserData)
    {
        std::cerr << pMessage << std::endl;
        return VK_FALSE;
    }
#endif

namespace simpleVulkan
{

    Application::Application() : m_validate(false) {}
    Application::~Application(){}
    bool Application::create(std::string name,uint32_t width,uint32_t height, bool validate)
    {
        m_name = name;
        m_width = width;
        m_height = height;
		m_validate = validate;
        if(!InitGLFW(m_name,m_width,m_height,m_window))
        {
            std::cerr << "failed to init GLFW" << std::endl;
            return false;
        }
        auto&& extensions = createInstanceExtensions();
        auto&& layers = createInstanceLayers();
        if(vk::Result::eSuccess != m_instance.create(m_name,1,"SimpleVulkan",1,extensions,layers))
        {
            std::cerr << "failed to create vulkan instance" << std::endl;
            return false;
        }

#if ENABLE_VALIDATION
		if (getValidateFlag())
		{
				VkInstance instance = (m_instance.getVkInstance());
				/* Load VK_EXT_debug_report entry points in debug builds */
				PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
					reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
						(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
				PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT =
					reinterpret_cast<PFN_vkDebugReportMessageEXT>
						(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
				PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
					reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
						(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));

					/* Setup callback creation information */
				VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
				callbackCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
				callbackCreateInfo.pNext       = nullptr;
				callbackCreateInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT |
												 VK_DEBUG_REPORT_WARNING_BIT_EXT |
												 VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
				callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
				callbackCreateInfo.pUserData   = nullptr;

				/* Register the callback */
				VkDebugReportCallbackEXT callback;
				VkResult result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);
				if (result != VK_SUCCESS) {
					std::cerr << "failed to create debug report callback!" << std::endl;
					exit(-1);
				}
		}
#endif
        return initialize();
    }

    void Application::destroy()
    {
        finalize();
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    bool Application::run()
    {
        while(glfwWindowShouldClose(m_window) == false)
        {
            glfwPollEvents();
            if(!render())
            {
                return false;
            }
            std::this_thread::sleep_for(m_interval);
        }
        return true;
    }

    std::chrono::microseconds Application::getInterval()
    {
        return m_interval;
    }
    void Application::setInterval(std::chrono::milliseconds interval)
    {
        m_interval = interval;
    }
    uint32_t Application::getWidth()
    {
        return m_width;
    }
    uint32_t Application::getHeight()
    {
        return m_height;
    }
    void Application::setSize(uint32_t width,uint32_t height)
    {
        m_width = width;
        m_height = height;
        glfwSetWindowSize(m_window,m_width,m_height);
    }
	bool Application::getValidateFlag()
	{
		return m_validate;
	}
	void Application::setValidateFlag(bool validate)
	{
		m_validate = validate;
	}
    
    GLFWwindow* Application::getWindow()
    {
        return m_window;
    }

    void Application::closeWindow()
    {
        glfwSetWindowShouldClose(m_window,0);
    }

    Instance* Application::getInstance()
    {
        return &m_instance;
    }
    
    bool Application::InitGLFW(const std::string& name,uint32_t width,uint32_t height,GLFWwindow*& resultWindow)
    {
        glfwInit();
        if(!glfwVulkanSupported())
        {
            return false;
        }
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
        resultWindow = glfwCreateWindow(width,height,name.c_str(),nullptr,nullptr);
        return true;
    }

    std::vector<const char*> Application::createInstanceExtensions()
    {
        unsigned int glfwExtensionsCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
        std::vector<const char*> extensions(glfwExtensionsCount);
        for(size_t i=0;i<glfwExtensionsCount;++i)
        {
            extensions[i]=glfwExtensions[i];
        }

		if (m_validate)
		{
			extensions.push_back("VK_EXT_debug_report");
		}
        return std::move(extensions);
    }
    
    std::vector<const char*> Application::createInstanceLayers()
    {
        std::vector<const char*> layers;

        if(m_validate)
        {
            layers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        return std::move(layers);
    }

}

