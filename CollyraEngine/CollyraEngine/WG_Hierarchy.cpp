#include "WG_Hierarchy.h"
#include "Application.h"
#include "M_UIManager.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "C_Transform.h"
#include "Component.h"

WG_Hierarchy::WG_Hierarchy(bool isActive) : WindowGroup(WG_HIERARCHY, isActive), rootPointer(nullptr),
flag(ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth), selected(-1), popUpSelected(-1),
changeSceneNamePopUp(false)
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

	HandlePopUp();

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

	//Handle Input
	HandleNodeInput(gameObject);
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

	if (gameObject->GetUid() == ROOT_ID)
		flag |= ImGuiTreeNodeFlags_DefaultOpen;

	if (gameObject->children.size() == 0)
		flag |= ImGuiTreeNodeFlags_Leaf;


	if (gameObject->GetUid() == selected)
		flag |= ImGuiTreeNodeFlags_Selected;

}

void WG_Hierarchy::HandleNodeInput(GameObject* currentGo)
{
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		if (currentGo->GetUid() != ROOT_ID)
		{
			App->uiManager->SetFocusedGameObject(currentGo->GetUid());
		}
	}
	else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		popUpSelected = currentGo->GetUid();
	}

}

void WG_Hierarchy::HandleDragAndDrop(GameObject* currentGo)
{
	//Drop on Target --------------
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			GameObject* gameObjectBuffer = (GameObject*)(payload->Data);
			if (gameObjectBuffer != nullptr)
			{
				GameObject* transportedGameObject = App->scene->GetGameObject(gameObjectBuffer->GetUid());
				if (transportedGameObject != nullptr)
				{
					DrpGameObject(transportedGameObject, currentGo);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (currentGo->GetUid() != ROOT_ID)
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

	moved->SetParent(objective);
}

void WG_Hierarchy::HandlePopUp()
{
	if (changeSceneNamePopUp == true)
	{
		PopUpSceneName();
	}
	else
	{
		PopUpHierarchy();
	}

}

void WG_Hierarchy::PopUpHierarchy()
{
	if (popUpSelected == -1)
		return;

	ImGui::OpenPopup("Hierarchy PopUp");
	if (ImGui::BeginPopup("Hierarchy PopUp"))
	{
		if (popUpSelected != ROOT_ID)
		{
			if (ImGui::MenuItem("Delete"))
			{
				App->scene->DeleteGameObject(popUpSelected);
				popUpSelected = -1;
			}

			if (ImGui::MenuItem("Create Empty"))
			{
				App->scene->CreateGameObject("", App->scene->GetGameObject(popUpSelected));
			}
		}
		else
		{
			if (ImGui::MenuItem("Change Name"))
			{
				changeSceneNamePopUp = true;
				strcpy_s(sceneName, 256, App->scene->GetSceneName().c_str());
			}

			if (ImGui::MenuItem("Create Empty"))
			{
				App->scene->CreateGameObject("", App->scene->GetGameObject(popUpSelected));
			}
		}

		ImGui::EndPopup();
	}
	ImGui::CloseCurrentPopup();

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) ||
		ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		popUpSelected = -1;
		return;
	}
}

void WG_Hierarchy::PopUpSceneName()
{
	ImGui::OpenPopup("Change Scene Name");
	if (ImGui::BeginPopupModal("Change Scene Name"))
	{
		static ImGuiInputTextFlags inspectorNameFlags = ImGuiInputTextFlags_EnterReturnsTrue;

		bool enterPressed = ImGui::InputText("##sceneNameChange", sceneName, 256, inspectorNameFlags);

		if (ImGui::Button("CHANGE") || enterPressed)
		{
			App->scene->SetSceneName(sceneName);
			App->scene->SaveScene();
			ImGui::CloseCurrentPopup();
			changeSceneNamePopUp = false;
		}

		ImGui::SameLine();

		if (ImGui::Button("CANCEL"))
		{
			ImGui::CloseCurrentPopup();
			changeSceneNamePopUp = false;
		}

		ImGui::EndPopup();
	}
	ImGui::CloseCurrentPopup();

}

void WG_Hierarchy::NewFocusedGameObject(int id)
{
	selected = id;
}

void WG_Hierarchy::OnDestroyedId(int id)
{
	if (id == selected)
	{
		selected = -1;
	}

	if (id == popUpSelected)
	{
		popUpSelected = -1;
	}
}
