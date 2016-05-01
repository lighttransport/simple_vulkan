#include<vector>
#include<string>
#include<chrono>
#include<GLFW/glfw3.h>
#include"instance.h"

namespace simpleVulkan
{
    class Application
    {
   private:
       std::string m_name;
       uint32_t m_width;
       uint32_t m_height;
       GLFWwindow* m_window;
       std::chrono::milliseconds m_interval;
	   bool m_validate; // Enable Vulkan validation layer + debug report?
       simpleVulkan::Instance m_instance;

   public:
       Application();
       virtual ~Application();

       bool create(std::string name = "SimpleVulkan",uint32_t width = 400,uint32_t height = 400, bool validate = false);
       void destroy();
       bool run();
   protected:
       std::chrono::microseconds getInterval();
       void setInterval(std::chrono::milliseconds interval);
       uint32_t getWidth();
       void setSize(uint32_t width,uint32_t height);
       uint32_t getHeight();
       void setHeight(uint32_t height);
	   bool getValidateFlag();
	   void setValidateFlag(bool validate);
       GLFWwindow* getWindow();
       void closeWindow();
       Instance* getInstance();
   private:
       virtual bool InitGLFW(const std::string& name,uint32_t width,uint32_t height,GLFWwindow*& result);
       virtual std::vector<const char*> createInstanceExtensions();
       virtual std::vector<const char*> createInstanceLayers();

       virtual bool initialize() = 0;
       virtual void finalize() = 0;
       virtual bool render() = 0;
    };
}
