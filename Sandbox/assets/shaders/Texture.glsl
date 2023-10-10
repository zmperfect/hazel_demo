// Basic Texture Shader

//顶点着色器
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;//位置
layout(location = 1) in vec4 a_Color;//颜色
layout(location = 2) in vec2 a_TexCoord;//纹理
layout(location = 3) in float a_TexIndex;//纹理索引
layout(location = 4) in float a_TilingFactor;//平铺因子

uniform mat4 u_ViewProjection;//视图投影矩阵

out vec4 v_Color;//颜色
out vec2 v_TexCoord;//纹理坐标
out float v_TexIndex;//纹理索引
out float v_TilingFactor;//平铺因子

void main()
{
	v_Color = a_Color;//颜色
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);//变换
}

//片段着色器
#type fragment
#version 330 core

layout(location = 0) out vec4 color;//颜色

in vec4 v_Color;//颜色
in vec2 v_TexCoord;//纹理坐标
in float v_TexIndex;//纹理索引
in float v_TilingFactor;//平铺因子

uniform sampler2D u_Textures[32];//采样器数组

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;//纹理采样
}