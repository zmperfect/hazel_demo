#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL //����ʵ���Թ���
#include <glm/gtx/quaternion.hpp>//��Ԫ��

#include "ScriptableEntity.h"
#include "SceneCamera.h"
#include "Hazel/Renderer/Texture.h"


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
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };//ƽ��
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };//��ת
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };//����

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)//���캯��
            : Translation(translation) {}

		glm::mat4 GetTransform() const//��ȡ�任����
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));//��Ԫ��ת��Ϊ��ת����

            return glm::translate(glm::mat4(1.0f), Translation)//ƽ��
                * rotation//��ת
                * glm::scale(glm::mat4(1.0f), Scale);//����
        }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)//���캯��
            : Color(color) {}
	};

	struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true; //�Ƿ�Ϊ�������
		bool FixedAspectRatio = false;//�Ƿ�̶����߱�

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