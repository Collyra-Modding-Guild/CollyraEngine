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


		ImGui::Checkbox("|", &focusedGameObject->active);
		ImGui::SameLine();

		char inspectorName[30];
		strcpy_s(inspectorName, 30, focusedGameObject->GetName().c_str());

		ImGuiInputTextFlags inspectorNameFlags = ImGuiInputTextFlags_EnterReturnsTrue;

		if (ImGui::InputText("##name", inspectorName, 30, inspectorNameFlags))
			focusedGameObject->SetName(inspectorName);

		ImGui::SameLine();

		bool staticGameObject = false;
		if (ImGui::Checkbox("Static", &staticGameObject))
			LOG("Static gameObject not Implemented");	

		

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Separator();
		
		float3 selectedPosition = focusedGameObject->GetComponent<C_Transform>()->GetPosition();
		Quat selectedRotation = focusedGameObject->GetComponent<C_Transform>()->GetRotation();
		float3 selectedScale = focusedGameObject->GetComponent<C_Transform>()->GetScale();


	

		bool transformActive = true;
		if (ImGui::Checkbox("", &transformActive))
			LOG("Transform is always active.")

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{

			// POSITION - - - - - - - - - - - - - - - - - - - - - - 
			ImGui::Spacing();

			ImGui::Text("Position   ");
			ImGui::SameLine();

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##posX", &selectedPosition.x, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##posY", &selectedPosition.y, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##posZ", &selectedPosition.z, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			// ROTATION - - - - - - - - - - - - - - - - - - - - - - 
			ImGui::Spacing();

			ImGui::Text("Rotation   ");
			ImGui::SameLine();

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##rotX", &selectedRotation.x, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##rotY", &selectedRotation.y, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##rotZ", &selectedRotation.z, 1.0f, 0.0f, 0.0f, "%f", 1.0f);


			// SCALE  - - - - - - - - - - - - - - - - - - - - - - 
			ImGui::Spacing();

			ImGui::Text("Scale      ");
			ImGui::SameLine();

			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##scaX", &selectedScale.x, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##scaY", &selectedScale.y, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			ImGui::DragFloat("##scaZ", &selectedScale.z, 1.0f, 0.0f, 0.0f, "%f", 1.0f);

			ImGui::Spacing();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Mesh          ");
			ImGui::SameLine();
			ImGui::Text("Mesh Name");
			ImGui::Text("Mesh Path     ");
			ImGui::SameLine();
			ImGui::Text("Path");

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Text("Vertices      ");
			ImGui::SameLine();
			ImGui::Text("0");

			ImGui::Text("Indices       ");
			ImGui::SameLine();
			ImGui::Text("0");

			ImGui::Text("Normals       ");
			ImGui::SameLine();
			ImGui::Text("0");

			ImGui::Spacing();
			ImGui::Spacing();

		}
	
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		bool materialActive = true;
		if (ImGui::Checkbox("", &materialActive))
			LOG("Material is always active.")

		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Texture          ");
			ImGui::SameLine();
			ImGui::Text("Texture Name");
			ImGui::Text("Texture Path     ");
			ImGui::SameLine();
			ImGui::Text("Path");

		}

		ImGui::Spacing();
		ImGui::Separator();
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
