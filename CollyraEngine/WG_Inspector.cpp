#include "WG_Inspector.h"
#include "Application.h"
#include "M_Scene.h"
#include "M_Input.h"
#include "M_Resources.h"

#include "GameObject.h"

#include "Component.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Transform.h"


WG_Inspector::WG_Inspector(bool isActive) : WindowGroup(WG_INSPECTOR, isActive), focusedGameObject(nullptr),
focusedId(-1), currentTag(tags[0]), currentLayer(layers[0]), defaultTexture(false), savedTexture(-1)
{
}

WG_Inspector::~WG_Inspector()
{}

updateStatus WG_Inspector::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");

	if (focusedId != -1)
	{
		C_Mesh* mesh = focusedGameObject->GetComponent<C_Mesh>();
		C_Material* material = focusedGameObject->GetComponent<C_Material>();

		//Delete the selected item------------
		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
		{
			App->scene->DeleteGameObject(focusedGameObject);
			ImGui::End();
			return UPDATE_CONTINUE;
		}

		DrawHeaderGameObject();

		DrawTransformComponent(ImGuiTreeNodeFlags_DefaultOpen);


		if (mesh != nullptr)
			DrawMeshComponent(ImGuiTreeNodeFlags_DefaultOpen, mesh);

		if (material != nullptr)
			DrawMaterialComponent(ImGuiTreeNodeFlags_DefaultOpen, material);

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

void WG_Inspector::DrawTransformComponent(ImGuiTreeNodeFlags_ flag)
{

	float3 selectedPosition = focusedGameObject->GetComponent<C_Transform>()->GetPosition();
	float3 selectedRotation = focusedGameObject->GetComponent<C_Transform>()->GetRotationEuler();
	float3 selectedScale = focusedGameObject->GetComponent<C_Transform>()->GetScale();

	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Transform", flag))
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



}

void WG_Inspector::DrawMeshComponent(ImGuiTreeNodeFlags_ flag, C_Mesh* mesh)
{
	bool meshActive = mesh->IsActive();
	if (ImGui::Checkbox("##MeshActive", &meshActive))
		mesh->SetActive(meshActive);

	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Mesh", flag))
	{
		ImGui::Text("Mesh          ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "Mesh Name");
		ImGui::Text("Mesh Path     ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "Path");

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Vertices      ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->vertices.size());

		ImGui::Text("Indices       ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->indices.size());

		ImGui::Text("Normals       ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->normals.size());

		ImGui::Spacing();
		ImGui::Spacing();

	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

}

void WG_Inspector::DrawMaterialComponent(ImGuiTreeNodeFlags_ flag, C_Material* material)
{

	bool materialActive = material->IsActive();
	if (ImGui::Checkbox("##MMaterialActive", &materialActive))
		material->SetActive(materialActive);

	ImGui::SameLine();

	if (ImGui::CollapsingHeader("Material", flag))
	{
		ImGui::Text("Texture          ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "Texture Name");
		ImGui::Text("Texture Path     ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "Path");

		ImGui::Spacing();
		ImGui::Spacing();

		ImVec2 imgPreview = ImVec2(200, 200);

		// We Flip the UVs
		ImVec2 uvMin = ImVec2(0.0f, 1.0f);                 // Top-left
		ImVec2 uvMax = ImVec2(1.0f, 0.0f);                 // Lower-right

	
		ImGui::Text("Set Default Texture");
		ImGui::SameLine();
		if (ImGui::Checkbox("##defaultTex", &defaultTexture))
		{
			if (savedTexture == -1)
				savedTexture = material->GetTexture();

			if (defaultTexture)
				material->SetTexture(App->resources->defaultTextureId, false);
			else
				material->SetTexture(savedTexture, false);
		}

		ImGui::Spacing();
		ImGui::Spacing();


		ImGui::Text("Albedo");
		ImGui::Spacing();
		ImGui::Indent();
		ImGui::Image((ImTextureID)material->GetTexture(), imgPreview, uvMin, uvMax);
		ImGui::Unindent();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

void WG_Inspector::DrawHeaderGameObject()
{
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


	ImGui::Text("Tag");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(110.0f);
	if (ImGui::BeginCombo("##tags", currentTag))
	{
		for (int n = 0; n < TAGS_NUM; n++)
		{
			bool is_selected = (currentTag == tags[n]);
			if (ImGui::Selectable(tags[n], is_selected))
				currentTag = tags[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	ImGui::Text("Layer");
	ImGui::SameLine();

	ImGui::SetNextItemWidth(110.0f);
	if (ImGui::BeginCombo("##layers", currentLayer))
	{
		for (int n = 0; n < LAYERS_NUM; n++)
		{
			bool is_selected = (currentLayer == layers[n]);
			if (ImGui::Selectable(layers[n], is_selected))
				currentLayer = layers[n];
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Separator();
}
