// Basic Texture Shader

//顶点着色器
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;//位置
layout(location = 1) in vec2 a_TexCoord;//纹理

uniform mat4 u_ViewProjection;//视图投影矩阵
uniform mat4 u_Transform;//变换矩阵

out vec2 v_TexCoord;//纹理坐标

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);//变换
}

//片段着色器
#type fragment
#version 330 core

layout(location = 0) out vec4 color;//颜色

in vec2 v_TexCoord;//纹理坐标

uniform sampler2D u_Texture;//采样器

void main()
{
	color = texture(u_Texture, v_TexCoord);
}