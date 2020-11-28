#include "WG_Assets.h"
#include "Globals.h"
#include "Application.h"
#include "M_Resources.h"
#include "M_UIManager.h"
#include "M_FileManager.h"
#include "R_Resource.h"
#include "M_Scene.h"

#include <string>

WG_Assets::WG_Assets(bool isActive) : WindowGroup(WG_TYPE::WG_ASSETS, isActive)
{}

WG_Assets::~WG_Assets()
{}

updateStatus WG_Assets::Update()
{
	ImGui::Begin("Assets");

	std::string toLoad = App->uiManager->DrawDirectoryRecursive(ASSETS_FOLDER, true);

	if (toLoad != "")
	{
		uint id = App->resources->ImportResourceFromAssets(toLoad.c_str());

		if (id > 0)
		{
			Resource* loadedResource = App->resources->LoadResource(id);


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

	ImGui::End();

	return UPDATE_CONTINUE;

}

void WG_Assets::Cleanup()
{}
