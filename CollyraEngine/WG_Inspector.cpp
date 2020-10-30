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
focusedId(-1), currentTag(tags[0]), currentLayer(layers[0])
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

		//Delete the selected item - - - - - - - - - - - -
		if(DeleteGameObject(SDL_SCANCODE_DELETE))
			return UPDATE_CONTINUE;


		// Draw Inspector Structure  - - - - - - - - - - - -

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
	App->scene->focusedGameObject = this->focusedGameObject;

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
		ImGui::DragFloat("##posX", &selectedPosition.x);

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##posY", &selectedPosition.y);

		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##posZ", &selectedPosition.z);

		// ROTATION - - - - - - - - - - - - - - - - - - - - - - 
		ImGui::Spacing();

		ImGui::Text("Rotation   ");
		ImGui::SameLine();

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##rotX", &selectedRotation.x);

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##rotY", &selectedRotation.y);

		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##rotZ", &selectedRotation.z);


		// SCALE  - - - - - - - - - - - - - - - - - - - - - - 
		ImGui::Spacing();

		ImGui::Text("Scale      ");
		ImGui::SameLine();

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##scaX", &selectedScale.x);

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##scaY", &selectedScale.y);

		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.0f);
		ImGui::DragFloat("##scaZ", &selectedScale.z);

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

		char meshName[100];
		strcpy_s(meshName, 100, mesh->GetMeshName().c_str());

		ImGui::Text("Mesh          ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, meshName);

		char meshPath[100];
		strcpy_s(meshPath, 100, mesh->GetMeshPath().c_str());

		ImGui::Text("Mesh Path     ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, meshPath);

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Vertices      ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->GetVerticesSize());

		ImGui::Text("Indices       ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->GetIndicesSize());

		ImGui::Text("Normals       ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, "%i", mesh->GetNormalsSize());

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Vertex Normals");
		ImGui::SameLine();
		bool drawNormalV = mesh->GetDrawingNormVertices(); // DRAW NORMAL VERTICES
		ImGui::Checkbox("##drawNormV", &drawNormalV);
		mesh->SetDrawingNormVertices(drawNormalV);

		ImGui::SameLine();

		ImGui::Text("Face Normals");
		ImGui::SameLine();
		bool drawNormalF = mesh->GetDrawingNormFaces(); // DRAW NORMAL FACES
		ImGui::Checkbox("##drawNormF", &drawNormalF);
		mesh->SetDrawingNormFaces(drawNormalF);

		ImGui::Text("Wire Draw");
		ImGui::SameLine();
		bool drawWire = mesh->GetDrawingWire();
		ImGui::Checkbox("##drawWire", &drawWire); // DRAW WIRE
		mesh->SetDrawingWire(drawWire);

		ImGui::SameLine();

		ImGui::Text("Face Draw");
		ImGui::SameLine();
		bool drawFaces = mesh->GetDrawingFaces();
		ImGui::Checkbox("##drawFaces", &drawFaces); // DRAW FACES
		mesh->SetDrawingFaces(drawFaces);

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
		char materialName[100];
		strcpy_s(materialName, 100, material->GetMaterialName().c_str());

		ImGui::Text("Material Color:          ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, materialName);

		Color myColor = material->GetColor();
		myColor.r *= 255;
		myColor.g *= 255;
		myColor.b *= 255;
		myColor.a *= 255;

		ImGui::Text("R");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.0f);
		ImGui::DragFloat("##colorR", &(myColor.r));

		ImGui::SameLine();
		ImGui::Text("G");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.0f);
		ImGui::DragFloat("##colorG", &myColor.g);

		ImGui::SameLine();
		ImGui::Text("B");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.0f);
		ImGui::DragFloat("##colorB", &myColor.b);

		ImGui::SameLine();
		ImGui::Text("A");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.0f);
		ImGui::DragFloat("##colorA", &myColor.a);

		ImGui::Text("Texture          ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, materialName);

		char materialPath[100];
		strcpy_s(materialPath, 100, material->GetMaterialPath().c_str());

		ImGui::Text("Texture Path     ");
		ImGui::SameLine();
		ImGui::TextColored({ 0 , 255 , 255 , 100 }, materialPath);

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Set Default Texture");
		ImGui::SameLine();
		bool usingDefaultTexture = material->GetDefaultTextureUsage();
		ImGui::Checkbox("##defaultTex", &usingDefaultTexture);
		material->SetUseDefaultTexture(usingDefaultTexture);

		ImGui::Spacing();
		ImGui::Spacing();

		ImVec2 imgPreview = ImVec2(200, 200); // TEXTURE PREVIEW SIZE

		// We Flip the UVs
		ImVec2 uvMin = ImVec2(0.0f, 1.0f);                 // Top-left
		ImVec2 uvMax = ImVec2(1.0f, 0.0f);                 // Lower-right

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
	if (ImGui::BeginCombo("##tags", currentTag)) // Check TAGS list and selected Tag
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
	if (ImGui::BeginCombo("##layers", currentLayer)) // Check LAYERS list and selected Layer
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

bool WG_Inspector::DeleteGameObject(int key)
{
	if (App->input->GetKey(key) == KEY_DOWN)
	{
		App->scene->DeleteGameObject(focusedGameObject);
		ImGui::End();
		return true;
	}

	return false;
}
