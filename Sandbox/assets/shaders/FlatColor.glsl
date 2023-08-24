// Flat Color Shader

//顶点着色器
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;//位置

uniform mat4 u_ViewProjection;//视图投影矩阵
uniform mat4 u_Transform;//变换矩阵

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);//变换
}

//片段着色器
#type fragment
#version 330 core

layout(location = 0) out vec4 color;//颜色

uniform vec4 u_Color;//颜色

void main()
{
	color = u_Color;
}