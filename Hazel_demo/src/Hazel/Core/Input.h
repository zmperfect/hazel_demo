#pragma once

#include <glm/glm.hpp>

#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

namespace Hazel {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);//是否按下某个键
		static bool IsMouseButtonPressed(MouseCode button);//是否按下某个鼠标键
		static glm::vec2 GetMousePosition();//获取鼠标位置
		static float GetMouseX();//获取鼠标X坐标
		static float GetMouseY();//获取鼠标Y坐标
	};

}