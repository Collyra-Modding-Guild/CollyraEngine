#include "WG_Inspector.h"
#include "Application.h"
#include "M_Scene.h"
#include "M_Input.h"

#include "GameObject.h"

#include "Component.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Transform.h"


WG_Inspector::WG_Inspector(bool isActive) : WindowGroup(WG_INSPECTOR, isActive), focusedGameObject(nullptr),
focusedId(-1)
{

}

WG_Inspector::~WG_Inspector()
{

}

updateStatus WG_Inspector::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");

	if (focusedId != -1)
	{
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			App->scene->DeleteGameObject(focusedGameObject);
			ImGui::End();
			return UPDATE_CONTINUE;
		}

		ImGui::Text(focusedGameObject->GetName().c_str());

		float3 pep = focusedGameObject->GetComponent<C_Transform>()->GetPosition();

		ImGui::Text("%f.%f.%f", pep.x, pep.y, pep.z);
		true;
	}

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Inspector::Cleanup()
{}

void WG_Inspector::SetGameObject(uint focusedGameObject)
{
	this->focusedGameObject = App->scene->GetGameObject(focusedGameObject);

	if (this->focusedGameObject == nullptr)
		focusedId = -1;
	else
		focusedId = focusedGameObject;
}

int WG_Inspector::GetFocusedGameObjectId() const
{
	return focusedId;
}

void WG_Inspector::SetGameObject(GameObject* focusedGameObject)
{
	if (focusedGameObject != nullptr)
	{
		this->focusedGameObject = focusedGameObject;
		focusedId = focusedGameObject->GetId();
	}
}

GameObject* WG_Inspector::GetGameObject() const
{
	return focusedGameObject;
}

void WG_Inspector::OnDestroyedId(uint destroyedGameObject)
{
	if (destroyedGameObject == focusedId)
	{
		focusedId = -1;
		focusedGameObject = nullptr;
	}
}
