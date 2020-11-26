#include "M_Resources.h"
#include "Application.h"
#include "M_FileManager.h"
#include "M_UIManager.h"
#include "M_Scene.h"

#include "MeshLoader.h"
#include "TextureLoader.h"
#include "C_Material.h"

#include "PathNode.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "Assimp/include/mesh.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

//Resources ----
#include "R_Resource.h"
#include "R_Model.h"
#include "R_Material.h"
#include "R_Mesh.h"
#include "R_Scene.h"
#include "R_Texture.h"

//Importers-----
#include "SceneLoader.h"
#include "TextureLoader.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"

#include "JsonConfig.h"


M_Resources::M_Resources(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), defaultTextureId(-1)
{}

M_Resources::~M_Resources()
{}

bool M_Resources::Awake()
{
	//Loaders Initialization ---------------------
	MeshLoader::Init();
	TextureLoader::Init();

	randomGen = LCG::LCG();

	return true;
}

bool M_Resources::Start()
{
	//Load from Start Demo-------
	defaultTextureId = TextureLoader::LoadDefaultTexture();

	ImportResource("Assets/Meshes/house.fbx");

	return true;
}

bool M_Resources::CleanUp()
{
	//Loaders CleanUp----------------------------
	MeshLoader::CleanUp();
	TextureLoader::CleanUp();

	return true;
}

uint M_Resources::ImportResource(const char* path)
{
	//Paths set up------------------------
	std::string normalizedPath = App->physFS->NormalizePath(path);
	std::string relativePath = "";

	//In case the file we are tying to import is from anywhere on the disk (not the project folder)
	if (!App->physFS->Exists(normalizedPath.c_str()))
	{
		LOG("Importing Resource not found in Assets/... Searching in the disk...")
			relativePath = DuplicateFile(normalizedPath.c_str());

		if (relativePath == "")
		{
			LOG("Invalid Path!!! Could not load from %s", path);
			return 0;
		}
	}
	else
		relativePath = normalizedPath;

	R_TYPE resourceType = GetResourceTypeFromExtension(normalizedPath.c_str());

	if (resourceType != R_TYPE::NO_TYPE)
	{
		Resource* newResource = CreateResource(resourceType);

		char* fileBuffer = nullptr;
		uint64 fileSize = 0;
		fileSize = App->physFS->Load(relativePath.c_str(), &fileBuffer);

		switch (resourceType)
		{
		case (R_TYPE::TEXTURE):				TextureLoader::Import(normalizedPath.c_str(), &fileBuffer, fileSize); break;
		case (R_TYPE::MODEL):				ImportModel(normalizedPath.c_str(), &fileBuffer, fileSize, (R_Model*)newResource); break;
		}

		SaveResource(newResource, path);

		uint newResourceID = newResource->GetUid();
		RELEASE_ARRAY(fileBuffer);
		UnloadResource(newResource->GetUid());

		return newResourceID;

		/*
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

		}
		else
			LOG("Ignored: %s | File type unsupported %s", path, extension.c_str());
			*/
	}
	else
	{
		LOG("Could not import %s! Extension unknown or file not found", path);
	}


}

Resource* M_Resources::RequestResource(uint id)
{
	Resource* resource = nullptr;

	//Search if the resource is loaded in mem
	std::map<uint, Resource*>::iterator resourcesIt = resourceMap.find(id);
	if (resourcesIt != resourceMap.end())
	{
		resourcesIt->second->referenceCount++;
		return resourcesIt->second;
	}

	// If not, search in library & load it
	//TODO: This HAS to be optimized
	PathNode allFiles = App->physFS->GetAllFiles(LIBRARY_PATH, nullptr, nullptr);
	std::string foundId = App->physFS->FindInPathNode(std::to_string(id).c_str(), allFiles);

	if (foundId.c_str() == "")
	{
		//TODO: Search in Assets .neta & try to find the id
		RELEASE(resource);
		return nullptr;
	}
	else
	{
		LoadResource(foundId.c_str(), id);
	}

	return resource;
}

Resource* M_Resources::LoadResource(const char* path, uint id)
{
	Resource* ret = nullptr;

	char* buffer = nullptr;
	uint size = App->physFS->Load(path, &buffer);
	if (size == 0)
	{
		return ret;
	}

	std::string extension;
	App->physFS->SplitFilePath(path, nullptr, nullptr, nullptr, &extension);

	R_TYPE resourceType = App->physFS->GetTypeFromExtension(extension.c_str());

	switch (resourceType)
	{
	case (R_TYPE::MESH):
	{
		ret = new R_Mesh(id);
		MeshLoader::Private::LoadMesh((R_Mesh*)ret, &buffer, id);
		break;
	}
	case (R_TYPE::TEXTURE):
	{
		ret = new R_Texture(id);
		R_Texture* tmp = (R_Texture*)ret;
		tmp->textureId = TextureLoader::Load(buffer, size, (R_Texture*)ret);
		break;
	}
	case (R_TYPE::MATERIAL):
	{
		ret = new R_Material(id);
		MaterialLoader::Load((R_Material*)ret, buffer);
		break;
	}
	case (R_TYPE::MODEL):
	{
		ret = new R_Model(id);
		ModelLoader::Load((R_Model*)ret, buffer);
		break;
	}
	case (R_TYPE::SCENE):
	{
		//Importer::Scenes::Load(buffer, (R_Scene*)resource);
		break;
	}
	}

	RELEASE_ARRAY(buffer);
	ret->referenceCount++;
	return ret;
}

Resource* M_Resources::CreateResource(R_TYPE rType)
{
	Resource* ret = nullptr;

	//TODO: Check if the resource already exists, if it does, get his id & delete the old one
	//TODO 2: Also, instances :)
	uint32 uId = GenerateId();

	switch (rType)
	{
	case R_TYPE::MODEL:ret = new R_Model(uId);	break;
	case R_TYPE::SCENE:	ret = new R_Scene(uId); break;
	case R_TYPE::MESH:ret = new R_Mesh(uId); break;
	case R_TYPE::TEXTURE:ret = new R_Texture(uId); break;
	case R_TYPE::MATERIAL: ret = new R_Material(uId); break;
	default:
		break;
	}

	if (ret != nullptr)
	{
		ret->SetName(std::to_string(uId));

		ret->SetLibraryPath(App->physFS->GetExtensionFolderLibraryFromType(rType).append(std::to_string(uId).append(App->physFS->GetInternalExtensionFromType(rType))));
		resourceMap.insert({ uId , ret });
	}

	return ret;
}

bool M_Resources::SaveResource(Resource* toSave, std::string assetsPath, bool saveMeta)
{
	char* buffer = nullptr;
	uint size = 0;

	switch (toSave->GetType())
	{
	case R_TYPE::MODEL:size = ModelLoader::Save((R_Model*)toSave, &buffer);	break;
		//case R_TYPE::SCENE:	ret = new R_Scene(uId); break;
	case R_TYPE::MESH: size = MeshLoader::Save((R_Mesh*)toSave, &buffer); break;
	case R_TYPE::TEXTURE:size = TextureLoader::Save(&buffer); break;
	case R_TYPE::MATERIAL: size = MaterialLoader::Save((R_Material*)toSave, &buffer); break;
	}

	if (size > 0)
	{
		App->physFS->Save(toSave->GetLibraryPath().c_str(), buffer, size);

		if (saveMeta == true)
			SaveMeta(toSave, assetsPath);

		RELEASE_ARRAY(buffer);

		return true;
	}

	return false;
}

bool M_Resources::SaveMeta(Resource* toSave, std::string assetsPath)
{
	JsonConfig config;

	config.AddNumber("ID", toSave->GetUid());
	config.AddString("Name", toSave->GetName().c_str());
	config.AddNumber("Type", (int)toSave->GetType());

	//TODO: This maybe isn't necessary (?)
	config.AddString("Library file", toSave->GetLibraryPath().c_str());

	//DATE
	uint64 currDate = App->physFS->GetCurrDate(assetsPath.c_str());
	config.AddNumber("Date", currDate);

	//TODO: If it is a model, maybe save its meshes & textures (?)

	char* buffer = nullptr;
	uint size = config.Serialize(&buffer);
	if (size > 0)
	{
		std::string path = assetsPath + ".meta";
		App->physFS->Save(path.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
		return true;
	}

	return false;
}

bool M_Resources::UnloadResource(uint32 idToDestroy)
{
	bool ret = false;
	std::map<uint, Resource*>::iterator it = resourceMap.find(idToDestroy);
	if (it != resourceMap.end())
	{
		RELEASE(it->second);
		resourceMap.erase(it);
		ret = true;
	}

	return ret;
}

R_TYPE M_Resources::GetResourceTypeFromExtension(const char* rPath)
{
	std::string extension = "";
	App->physFS->SplitFilePath(rPath, nullptr, nullptr, nullptr, &extension);

	extension = App->physFS->LowerCaseString(extension.c_str());

	if (extension == "fbx")
	{
		return R_TYPE::MODEL;
	}
	else if (extension == "png" || extension == "jpg" || extension == "dds" || extension == "tga")
	{
		return R_TYPE::TEXTURE;
	}
	else if (extension == "collscene")
	{
		return R_TYPE::SCENE;
	}
	else if (extension == "collmat")
	{
		return R_TYPE::MATERIAL;
	}


	return R_TYPE::NO_TYPE;
}

uint32 M_Resources::GenerateId()
{
	return randomGen.Int();
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

void M_Resources::ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel)
{
	const aiScene* scene = aiImportFileFromMemory(*buffer, bufferSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene)
	{
		//WARNING: assuming that all the mesh is made from triangles
		std::string fbxName = "";
		App->physFS->SplitFilePath(path, nullptr, nullptr, &fbxName, nullptr);
		//GameObject* sceneRoot = App->scene->CreateGameObject(fbxName.c_str(), nullptr);
		uint32 ID = resourceModel->AddModelNode(GenerateId(), fbxName.c_str(), float4x4::identity, App->scene->GetRoot()->GetUid());

		MeshLoader::Private::LoadAiSceneMeshes(scene, scene->mRootNode, path, resourceModel, ID);

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading aiScene %s", path);
	}
}
