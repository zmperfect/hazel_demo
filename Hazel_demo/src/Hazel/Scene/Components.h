#pragma once

#include <glm/glm.hpp>

#include "ScriptableEntity.h"
#include "SceneCamera.h"

namespace Hazel {

	struct TagComponent//��ǩ���
	{
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)//���캯��
            : Tag(tag) {}
    };

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

	struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true; //�Ƿ�Ϊ�������
		bool FixedAspectRatio = false;//�Ƿ�̶���߱�

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();//ʵ�����ű�
        void(*DestroyScript)(NativeScriptComponent*);//���ٽű�

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };//ʵ�����ű�
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };//���ٽű�
		}
	};
}