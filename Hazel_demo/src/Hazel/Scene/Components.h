#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	struct TransformComponent//�任���
	{
		glm::mat4 Transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)//���캯��
            : Transform(transform) {}

		operator glm::mat4& () { return Transform; }//��������ת�������
		operator const glm::mat4& () const { return Transform; }//��������ת�������
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)//���캯��
            : Color(color) {}
	};
}