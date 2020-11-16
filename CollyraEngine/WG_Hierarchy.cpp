#include "WG_Hierarchy.h"
#include "Application.h"
#include "M_UIManager.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "C_Transform.h"
#include "Component.h"

WG_Hierarchy::WG_Hierarchy(bool isActive) : WindowGroup(WG_HIERARCHY, isActive), rootPointer(nullptr),
flag(ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth), selected(-1)
{}

WG_Hierarchy::~WG_Hierarchy()
{}

updateStatus WG_Hierarchy::Start()
{
	rootPointer = App->scene->GetRoot();
	return UPDATE_CONTINUE;
}

updateStatus WG_Hierarchy::Update()
{
	ImGui::Begin("Hierarchy");

	CreateHierarchy(rootPointer);

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Hierarchy::Cleanup()
{
	rootPointer = nullptr;
}

void WG_Hierarchy::CreateHierarchy(GameObject* gameObject)
{
	//Reset the Flags
	SetTreeNodeFlags(gameObject);

	bool isNodeOpened = ImGui::TreeNodeEx(gameObject->GetName().c_str(), flag);

	//Check Clicked
	if (ImGui::IsItemClicked())
	{
		if (gameObject->GetId() != ROOT)
		{
			App->uiManager->SetFocusedGameObject(gameObject->GetId());
			selected = gameObject->GetId();
		}
	}

	HandleDragAndDrop(gameObject);

	//If the Node is opened, display the childs
	if (isNodeOpened)
	{
		for (int i = 0; i < gameObject->children.size(); i++)
		{
			CreateHierarchy(gameObject->children[i]);
		}

		ImGui::TreePop();
	}

}

void WG_Hierarchy::SetTreeNodeFlags(GameObject* gameObject)
{
	flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	if (gameObject->GetId() == ROOT)
		flag |= ImGuiTreeNodeFlags_DefaultOpen;

	if (gameObject->children.size() == 0)
		flag |= ImGuiTreeNodeFlags_Leaf;

	if (gameObject->GetId() == selected)
		flag |= ImGuiTreeNodeFlags_Selected;
}

void WG_Hierarchy::HandleDragAndDrop(GameObject* currentGo)
{
	//Drop on Target--------------
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			GameObject* gameObjectBuffer = (GameObject*)(payload->Data);
			if (gameObjectBuffer != nullptr)
			{
				GameObject* transportedGameObject = App->scene->GetGameObject(gameObjectBuffer->GetId());
				if (transportedGameObject != nullptr)
				{
					DrpGameObject(transportedGameObject, currentGo);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (currentGo->GetId() != ROOT)
	{
		//Get Source-----------------
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("GameObject", currentGo, sizeof(GameObject));
			if (currentGo->children.size() > 0)
			{
				ImGui::Text("%s (+ %i...)", currentGo->GetName().c_str(), currentGo->children.size());
			}
			else
			{
				ImGui::Text("%s", currentGo->GetName().c_str());
			}
			ImGui::EndDragDropSource();
		}
	}
}

void WG_Hierarchy::DrpGameObject(GameObject* moved, GameObject* objective)
{
	//If the moved is already child of the objective OR the objective is child of Moved
	if (moved->GetParent() == objective || objective->GetParent() == moved)
	{
		return;
	}

	moved->GetParent()->NotifyChildDeath(moved);

	objective->AddChildren(moved);

	moved->SetParent(objective);
}
