#include "WG_Scene.h"
#include "Application.h"
#include "M_Window.h"
#include "M_Renderer3D.h"
#include "M_Resources.h"
#include "M_FileManager.h"

#include "M_Scene.h"
#include "M_Camera3D.h"
#include "M_Input.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "R_Resource.h"

#include "MathGeoLib/include/Math/float4x4.h"

WG_Scene::WG_Scene(bool isActive) : WindowGroup(WG_CONFIG, isActive), usingGizmo(false), gizOperation(ImGuizmo::OPERATION::TRANSLATE),
																						 gizMode(ImGuizmo::MODE::LOCAL)
{
	ImGuizmo::Enable(true);
}

WG_Scene::~WG_Scene()
{}

updateStatus WG_Scene::Update()
{
	updateStatus ret = UPDATE_CONTINUE;

	ImGui::Begin("Scene");
	{
		// Using a Child allow to fill all the space of the window.
		// It also alows customization
		ImGui::BeginChild("GameRender");

		ImVec2 winSize = ImGui::GetWindowSize();
		windowPos = ImGui::GetWindowPos();

		if (winSize.x != windowSize.x || winSize.y != windowSize.y)
			OnResize(winSize);

		// We Flip the UVs
		ImVec2 uvMin = ImVec2(0.0f, 1.0f);                 // Top-left
		ImVec2 uvMax = ImVec2(1.0f, 0.0f);                 // Lower-right

		ImGui::Image((ImTextureID)App->renderer3D->GetTextureBuffer(), winSize, uvMin, uvMax);


		HandleGuizmo();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset"))
			{
				std::string assetStr = "";
				assetStr.resize(payload->DataSize);

				memcpy(&assetStr.at(0), payload->Data, payload->DataSize);

				if (assetStr != "")
				{
					uint id = App->resources->ImportResourceFromAssets(assetStr.c_str());

					if (id > 0)
					{
						Resource* loadedResource = App->resources->RequestResource(id);

						if (loadedResource != nullptr)
						{
							switch (loadedResource->GetType())
							{
							case R_TYPE::MESH:
								break;
							case R_TYPE::TEXTURE:
							{
								App->scene->SetResourceToGameObject(id, loadedResource->GetType());
							}
							break;
							case R_TYPE::MATERIAL:
								break;
							default:
								break;
							}

							App->resources->UnloadResource(id);
						}

					}
				}
			}
			
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LibFile"))
			{
				std::string libStr = "";
				libStr.resize(payload->DataSize);

				memcpy(&libStr.at(0), payload->Data, payload->DataSize);

				if (libStr != "")
				{
					std::string name;
					App->physFS->SplitFilePath(libStr.c_str(), nullptr, nullptr, &name);

					uint id = std::stoi(name);
					Resource* loadedResource = App->resources->RequestResource(id);

					if (loadedResource != nullptr)
					{
						switch (loadedResource->GetType())
						{
						case R_TYPE::MESH:
						{
							App->scene->SetResourceToGameObject(id, loadedResource->GetType());
						}
						break;
						case R_TYPE::TEXTURE:
						{
							App->scene->SetResourceToGameObject(id, loadedResource->GetType());
						}
						break;
						case R_TYPE::MATERIAL:
						{
							App->scene->SetResourceToGameObject(id, loadedResource->GetType());
						}
						break;
						default:
							break;
						}

						App->resources->UnloadResource(id);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}


		ImGui::EndChild();
	}

	ImGui::End();

	return ret;
}

void WG_Scene::Cleanup()
{}

void WG_Scene::OnResize(ImVec2 newWindowSize)
{
	windowSize.x = newWindowSize.x;
	windowSize.y = newWindowSize.y;

	App->renderer3D->UpdateCamera(newWindowSize.x, newWindowSize.y);
}

void WG_Scene::GetWindowSize(float& w, float& h)
{
	w = windowSize.x;
	h = windowSize.y;
}

void WG_Scene::GetWindowPosition(float& x, float& y)
{
	x = windowPos.x;
	y = windowPos.y;
}

void WG_Scene::HandleGuizmo()
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;

	GameObject* selected = App->scene->focusedGameObject;

	usingGizmo = ImGuizmo::IsOver();

	if (selected != nullptr) 
	{
		C_Transform* transformSelected = selected->GetComponent<C_Transform>();
		float4x4 transformMat = transformSelected->GetTGlobalTransform();
		float4x4 parentGlobalMat = selected->GetParent()->GetComponent<C_Transform>()->GetGlobalTransform();

		viewMatrix.Set(App->renderer3D->GetCurrentViewMatrix().ptr());
		projectionMatrix.Set(App->renderer3D->GetCurrentProjectionMatrix().ptr());
		
		ImGuizmo::SetDrawlist();

		float w, h;
		GetWindowSize(w, h);
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, w, h);

		if(App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			gizOperation = ImGuizmo::OPERATION::TRANSLATE;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			gizOperation = ImGuizmo::OPERATION::ROTATE;

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			gizOperation = ImGuizmo::OPERATION::SCALE;

		if (gizOperation == ImGuizmo::OPERATION::SCALE)
			gizMode = ImGuizmo::MODE::LOCAL;
		else
			gizMode = ImGuizmo::MODE::WORLD;

		ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), gizOperation, gizMode, transformMat.ptr());

		if (ImGuizmo::IsUsing()) 
		{
			transformSelected->UpdateGuizmoTransformation(transformMat.Transposed(), parentGlobalMat);
		}
	}
}

