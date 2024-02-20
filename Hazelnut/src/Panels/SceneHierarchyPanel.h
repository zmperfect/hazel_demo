#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext;  }//获取选择的实体
		void SetSelectedEntity(Entity entity);//设置选择的实体
	private:
		void DrawEntityNode(Entity entity);//绘制实体节点
		void DrawComponents(Entity entity);//绘制组件
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}