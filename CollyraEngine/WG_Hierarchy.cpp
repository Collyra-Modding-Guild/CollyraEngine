#include "WG_Hierarchy.h"
#include "Application.h"
#include "M_UIManager.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "C_Transform.h"
#include "Component.h"

WG_Hierarchy::WG_Hierarchy(bool isActive) : WindowGroup(WG_HIERARCHY, isActive), rootPointer(nullptr), 
flag(ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth), 
selected(-1)
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
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Hierarchy");

	CreateHierarchy(rootPointer);

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Hierarchy::Cleanup()
{
	rootPointer = nullptr;
}

void WG_Hierarchy::CreateHierarchy(const GameObject* gameObject)
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

void WG_Hierarchy::SetTreeNodeFlags(const GameObject* gameObject)
{
	flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (gameObject->GetId() == ROOT)
		flag |= ImGuiTreeNodeFlags_DefaultOpen;

	if (gameObject->children.size() == 0)
		flag |= ImGuiTreeNodeFlags_Leaf;

	if (gameObject->GetId() == selected)
		flag |= ImGuiTreeNodeFlags_Selected;
}