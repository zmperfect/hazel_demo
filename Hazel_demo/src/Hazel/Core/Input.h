#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

namespace Hazel {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);//�Ƿ���ĳ����
		static bool IsMouseButtonPressed(MouseCode button);//�Ƿ���ĳ������
		static std::pair<float, float> GetMousePosition();//��ȡ���λ��
		static float GetMouseX();//��ȡ���X����
		static float GetMouseY();//��ȡ���Y����
	};

}