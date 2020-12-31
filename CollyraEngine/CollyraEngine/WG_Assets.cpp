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
	ImGui::Text("-Double left click to spawn (except models, into selected GameObject) \n-Middle Click to delete");

	ImGui::Spacing();

	static std::vector<std::string> ignoreExt = { "meta" };

	std::string ret = App->uiManager->DrawDirectoryRecursiveOld(ASSETS_FOLDER, true, &ignoreExt, "Asset");

	if (ret != "")
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			LoadNewAsset(ret);
		}
		else if (ImGui::IsMouseClicked(2))
		{
			DeleteAsset(ret);
		}

	}

	ret = App->uiManager->DrawDirectoryRecursiveOld(LIBRARY_PATH, true, &ignoreExt, "LibFile");

	if (ret != "")
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			LoadNewResource(ret);
		}
		else if (ImGui::IsMouseClicked(2))
		{
			DeleteAsset(ret);
		}

	}

	ImGui::End();

	return UPDATE_CONTINUE;

}

void WG_Assets::Cleanup()
{}

void WG_Assets::LoadNewAsset(std::string& toLoad)
{
	uint id = App->resources->ImportResourceFromAssets(toLoad.c_str());

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

void WG_Assets::LoadNewResource(std::string& toLoad)
{
	std::string name;
	App->physFS->SplitFilePath(toLoad.c_str(), nullptr, nullptr, &name);

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

void WG_Assets::DeleteAsset(std::string& toDelete)
{
	App->physFS->DeleteFileIn(toDelete.c_str());
	App->resources->CheckAssetInMeta((toDelete + ".meta"), toDelete);
}


