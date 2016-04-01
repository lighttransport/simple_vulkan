#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout (binding = 0) uniform Transfrom
{
    mat2 matrix;
} transform;

void main() 
{
    //mat2 matrix = {{2.0,0.0},{0.0,2.0}};
	gl_Position = vec4(transform.matrix * inPos,0.0f,1.0f);
    outColor = vec4(0.0f,1.0f,1.0f,1.0f);
}
