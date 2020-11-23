#include "M_Resources.h"
#include "Application.h"
#include "M_FileManager.h"
#include "M_UIManager.h"
#include "M_Scene.h"

#include "MeshLoader.h"
#include "TextureLoader.h"
#include "C_Material.h"

//Resources ----
#include "R_Resource.h"

M_Resources::M_Resources(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), defaultTextureId(-1)
{}

M_Resources::~M_Resources()
{}

bool M_Resources::Awake()
{
	//Loaders Initialization ---------------------
	MeshLoader::Init();
	TextureLoader::Init();

	return true;
}

bool M_Resources::Start()
{
	//Load from Start Demo-------
	defaultTextureId = TextureLoader::LoadDefaultTexture();

	CreateMeshesInternal("Assets/Meshes/house.fbx");

	return true;
}

bool M_Resources::CleanUp()
{
	//Loaders CleanUp----------------------------
	MeshLoader::CleanUp();
	TextureLoader::CleanUp();

	return true;
}

void M_Resources::ImportResourceInternal(const char* path)
{
	std::string normalizedPath = App->physFS->NormalizePath(path);
	std::string relativePath = "";
	std::string name = "";
	std::string extension = "";

	App->physFS->SplitFilePath(normalizedPath.c_str(), &relativePath, nullptr, &name, &extension);

	if (relativePath == "")
	{
		LOG("Importing Resource is not located in Assets/... Searching in the disk...")
		relativePath = DuplicateFile(normalizedPath.c_str());

		if (relativePath == "")
		{
			LOG("Invalid Path!!! Could not load from %s", path);
			return;
		}

	}

	extension = App->physFS->LowerCaseString(extension.c_str());

	if (extension == "fbx")
	{
		CreateMeshesInternal(relativePath.c_str());
	}
	else if (extension == "png" || extension == "dds" || extension == "jpg")
	{
		//TODO: Check if the texture has already been imported here
		char* compressedTextureBuffer = nullptr;
		std::string texturePath = LIBRARY_TEXTURES;
		uint compressedTextureSize;
		uint newTextureId = TextureLoader::Import(relativePath.c_str());

		if (newTextureId > 0)
		{
			compressedTextureSize = TextureLoader::Save(&compressedTextureBuffer, newTextureId);
			if (compressedTextureSize > 0)
			{
				texturePath.append(name);
				App->physFS->Save(texturePath.c_str(), compressedTextureBuffer, compressedTextureSize);
			}
			else
			{
				LOG("Texture %s could not be compressed!", relativePath.c_str());
				return;
			}
		}
		else
		{
			LOG("Texture %s could not be loaded!", relativePath.c_str());
			return;
		}

		int gameObjectTarget = App->uiManager->GetFocusedGameObjectId();
		if (gameObjectTarget != -1)
		{
			GameObject* focusedObject = App->scene->GetGameObject(gameObjectTarget);

			//TODO: This has to go the Save & Load Pipeline for materials!!!
			C_Material* cMaterial = focusedObject->GetComponent<C_Material>();

			if (cMaterial == nullptr)
			{
				cMaterial = (C_Material*)focusedObject->CreateComponent(COMPONENT_TYPE::MATERIAL);
			}

			cMaterial->SetTexture(TextureLoader::Load(compressedTextureBuffer, compressedTextureSize));
			cMaterial->SetTextureNameAndPath(std::string(name + "." + "dds").c_str(), texturePath.c_str());

			LOG("Texture loaded into focused Game Object!!");
		}
		else
			RELEASE_ARRAY(compressedTextureBuffer);

	}
	else
		LOG("Ignored: %s | File type unsupported %s", path, extension.c_str());
}

std::string M_Resources::DuplicateFile(const char* path)
{
	std::string normalizedPath = App->physFS->NormalizePath(path);

	std::string finalPath;

	if (App->physFS->ImportFile(normalizedPath.c_str(), finalPath))
	{
		return finalPath;
	}
	else
		return "";
}

void M_Resources::CreateMeshesInternal(const char* path)
{
	MeshLoader::Load(path);
}
