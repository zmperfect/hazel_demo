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

		Entity GetSelectedEntity() const { return m_SelectionContext;  }//��ȡѡ���ʵ��
		void SetSelectedEntity(Entity entity);//����ѡ���ʵ��
	private:
		void DrawEntityNode(Entity entity);//����ʵ��ڵ�
		void DrawComponents(Entity entity);//�������
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}