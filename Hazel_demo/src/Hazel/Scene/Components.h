#pragma once

#include <glm/glm.hpp>

#include "ScriptableEntity.h"
#include "SceneCamera.h"

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
        SceneCamera Camera;
        bool Primary = true; //是否为主摄像机
		bool FixedAspectRatio = false;//是否固定宽高比

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateFunction;//实例化脚本
		std::function<void()> DestroyInstanceFunction;//销毁脚本

		std::function<void(ScriptableEntity*)> OnCreateFunction;//创建脚本
		std::function<void(ScriptableEntity*)> OnDestroyFunction;//销毁脚本
		std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;//更新脚本

		template<typename T>
		void Bind()
		{
			InstantiateFunction = [&]()//实例化脚本
            {
                Instance = new T();
            };
			DestroyInstanceFunction = [&]()//销毁脚本
            {
                delete Instance;
                Instance = nullptr;
            };

            OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };//创建脚本,定义形式1, 省略使用this调用全局的Instance属性
            OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };//销毁脚本
            OnUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->OnUpdate(ts); };//更新脚本
		}

	};

}