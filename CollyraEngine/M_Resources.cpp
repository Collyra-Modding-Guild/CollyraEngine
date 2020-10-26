#include "M_Resources.h"
#include "Application.h"
#include "M_FileManager.h"

#include "MeshLoader.h"
#include "Mesh.h"
#include "TextureLoader.h"

M_Resources::M_Resources(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled)
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
	//Demo---
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
	std::string extension = "";

	App->physFS->SplitFilePath(normalizedPath.c_str(), &relativePath, nullptr, nullptr, &extension);

	//if (relativePath == "")
	//{
	//	LOG("Invalid Path!!! We only load assets from the project folder for now :P");
	//	return;
	//}

	extension = App->physFS->LowerCaseString(extension.c_str());

	if (extension == "fbx")
	{
		CreateMeshesInternal(relativePath.c_str());
	}
	else if (extension == "png" || extension == "dds")
	{
		LOG("There is no loading textures yet!");
	}

}

//void M_Resources::CreateMeshesExternal(const char* path)
//{
//	std::string normalizedPath = App->physFS->NormalizePath(path);
//
//	std::string finalPath;
//
//	//TODO: Check if the current file is already loaded
//	if (App->physFS->ImportFile(normalizedPath.c_str(), finalPath))
//	{
//		CreateMeshesInternal(finalPath.c_str());
//	}
//}

void M_Resources::CreateMeshesInternal(const char* path)
{
	MeshLoader::Load(path);
}
