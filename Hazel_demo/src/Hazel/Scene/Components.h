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

		std::function<void()> InstantiateFunction;//ʵ�����ű�
		std::function<void()> DestroyInstanceFunction;//���ٽű�

		std::function<void(ScriptableEntity*)> OnCreateFunction;//�����ű�
		std::function<void(ScriptableEntity*)> OnDestroyFunction;//���ٽű�
		std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;//���½ű�

		template<typename T>
		void Bind()
		{
			InstantiateFunction = [&]()//ʵ�����ű�
            {
                Instance = new T();
            };
			DestroyInstanceFunction = [&]()//���ٽű�
            {
                delete Instance;
                Instance = nullptr;
            };

            OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };//�����ű�,������ʽ1, ʡ��ʹ��this����ȫ�ֵ�Instance����
            OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };//���ٽű�
            OnUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->OnUpdate(ts); };//���½ű�
		}

	};

}