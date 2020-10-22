#include "M_Resources.h"
#include "Application.h"
#include "M_FileManager.h"

#include "MeshLoader.h"
#include "Mesh.h"
#include "TextureLoader.h"

M_Resources::M_Resources(MODULE_TYPE type, bool start_enabled) : Module(type, start_enabled)
{}

M_Resources::~M_Resources()
{}

bool M_Resources::Awake()
{
	//Loaders Initialization---------------------
	MeshLoader::Init();
	TextureLoader::Init();

	return true;
}

bool M_Resources::Start()
{
	//Demo---
	//CreateMeshesInternal("Assets/Meshes/warrior.fbx");

	return true;
}

bool M_Resources::CleanUp()
{
	//Loaders CleanUp----------------------------
	MeshLoader::CleanUp();
	TextureLoader::CleanUp();

	meshes.clear();

	return true;
}

void M_Resources::CreateMeshesExternal(const char* path)
{
	std::string normalizedPath = App->physFS->NormalizePath(path);

	std::string finalPath;

	//TODO: Check if the current file is already loaded
	if (App->physFS->ImportFile(normalizedPath.c_str(), finalPath))
	{
		CreateMeshesInternal(finalPath.c_str());
	}
}

void M_Resources::CreateMeshesInternal(const char* path)
{
	AddMeshes(MeshLoader::Load(path));
}

void M_Resources::AddMeshes(std::vector<Mesh>& newMeshes)
{
	if (newMeshes.size() > 0)
	{
		int meshesEnd = meshes.size();
		meshes.insert(meshes.end(), newMeshes.begin(), newMeshes.end());

		for (meshesEnd; meshesEnd < meshes.size(); meshesEnd++)
		{
			meshes[meshesEnd].SetTextureId(TextureLoader::LoadDefaultTexture());
		}
	}
}

std::vector<Mesh>* M_Resources::GetMeshes()
{
	return &meshes;
}
