// Basic Texture Shader

//顶点着色器
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;//位置
layout(location = 1) in vec4 a_Color;//颜色
layout(location = 2) in vec2 a_TexCoord;//纹理

uniform mat4 u_ViewProjection;//视图投影矩阵

out vec4 v_Color;//颜色
out vec2 v_TexCoord;//纹理坐标

void main()
{
	v_Color = a_Color;//颜色
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);//变换
}

//片段着色器
#type fragment
#version 330 core

layout(location = 0) out vec4 color;//颜色

in vec4 v_Color;//颜色
in vec2 v_TexCoord;//纹理坐标

uniform vec4 u_Color;//颜色
uniform float u_TilingFactor;//平铺因子
uniform sampler2D u_Texture;//采样器

void main()
{
	//color = texture(u_Texture, v_TexCoord * u_TilingFactor) * u_Color;//纹理采样
	color = v_Color;
}