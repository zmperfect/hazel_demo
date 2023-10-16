// Flat Color Shader

//������ɫ��
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;//λ��

uniform mat4 u_ViewProjection;//��ͼͶӰ����
uniform mat4 u_Transform;//�任����

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);//�任
}

//Ƭ����ɫ��
#type fragment
#version 330 core

layout(location = 0) out vec4 color;//��ɫ

uniform vec4 u_Color;//��ɫ

void main()
{
	color = u_Color;
}