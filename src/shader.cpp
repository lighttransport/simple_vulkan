#include"shader.h"
namespace simpleVulkan
{
    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
    }

    Result Shader::create(vk::Device device,size_t size,void* code)
    {
        m_device = device;
        vk::Result result;

        //init VertexShaderModuleCreateInfo
        vk::ShaderModuleCreateInfo shaderInfo;
        shaderInfo.flags(vk::ShaderModuleCreateFlagBits());
        shaderInfo.codeSize(size);
        shaderInfo.pCode(reinterpret_cast<uint32_t*>(code));

        //create VertexShader
        result = m_device.createShaderModule(&shaderInfo,nullptr,&m_shaderModule);
        return result;
    }

    void Shader::destroy()
    {
        m_device.destroyShaderModule(m_shaderModule,nullptr);
    }

    vk::ShaderModule& Shader::getVkShaderModule()
    {
        return m_shaderModule;
    }
}
