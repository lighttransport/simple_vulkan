#include"application.h"
#include<vulkan/vk_cpp.h>
#include<iostream>

namespace simpleVulkan
{
    Application::Application(){}
    Application::~Application(){}
    bool Application::create(std::string windowName,uint32_t width,uint32_t height)
    {
        m_width = width;
        m_height = height;
        glfwInit();
        if(!glfwVulkanSupported())
        {
            return false;
        }
        glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
        m_window = glfwCreateWindow(m_width,m_height,"test_vulkan",nullptr,nullptr);
        unsigned int glfwExtensionsCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
        std::vector<const char*> extensions(glfwExtensionsCount);
        for(size_t i=0;i<glfwExtensionsCount;++i)
        {
            extensions[i]=glfwExtensions[i];
        }
        return initialize(extensions,m_window);
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
            usleep(m_interval);
        }
        return true;
    }

    useconds_t Application::getInterval()
    {
        return m_interval;
    }
    void Application::setInterval(useconds_t interval)
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
    void Application::closeWindow()
    {
        glfwSetWindowShouldClose(m_window,0);
    }

}
