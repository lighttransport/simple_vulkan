#include<vector>
#include<string>
#include<GLFW/glfw3.h>
#include<unistd.h>

namespace simpleVulkan
{
    class Application
    {
   private:
       uint32_t m_width = 400;
       uint32_t m_height = 400;
       GLFWwindow* m_window;
       useconds_t m_interval = 20;

   public:
       Application();
       virtual ~Application();

       bool create(std::string windowName,uint32_t width,uint32_t height);
       void destroy();
       bool run();
   protected:
       useconds_t getInterval();
       void setInterval(useconds_t interval);
       uint32_t getWidth();
       void setSize(uint32_t width,uint32_t height);
       uint32_t getHeight();
       void setHeight(uint32_t height); 
       void closeWindow();
   private:
       virtual bool initialize(
               const std::vector<const char*>& glfwExtensions,
               GLFWwindow* window) = 0;
       virtual void finalize() = 0;
       virtual bool render() = 0;
    };
}
