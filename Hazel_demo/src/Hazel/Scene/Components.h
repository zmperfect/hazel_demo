#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

	struct TagComponent//标签组件
	{
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)//构造函数
            : Tag(tag) {}
    };

	struct TransformComponent//变换组件
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)//构造函数
            : Transform(transform) {}

		operator glm::mat4& () { return Transform; }//重载类型转换运算符
		operator const glm::mat4& () const { return Transform; }//重载类型转换运算符
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)//构造函数
            : Color(color) {}
	};

	struct CameraComponent
    {
        Hazel::Camera Camera;
        bool Primary = true; //是否为主摄像机

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
		CameraComponent(const glm::mat4& projection)
            : Camera(projection) {}
    };

}