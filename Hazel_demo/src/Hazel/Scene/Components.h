#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL //启用实验性功能
#include <glm/gtx/quaternion.hpp>//四元数

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
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };//平移
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };//旋转
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };//缩放

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)//构造函数
            : Translation(translation) {}

		glm::mat4 GetTransform() const//获取变换矩阵
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));//四元数转换为旋转矩阵

            return glm::translate(glm::mat4(1.0f), Translation)//平移
                * rotation//旋转
                * glm::scale(glm::mat4(1.0f), Scale);//缩放
        }
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

		ScriptableEntity*(*InstantiateScript)();//实例化脚本
        void(*DestroyScript)(NativeScriptComponent*);//销毁脚本

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };//实例化脚本
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };//销毁脚本
		}
	};
}