#include "M_Resources.h"
#include "Application.h"
#include "M_FileManager.h"
#include "M_UIManager.h"
#include "M_Scene.h"

#include "MeshLoader.h"
#include "TextureLoader.h"
#include "C_Material.h"

#include "PathNode.h"
#include "JsonConfig.h"

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


M_Resources::M_Resources(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), defaultTextureId(-1), deleteResources(true), onlineIdUpdated{},
allLibFiles(), allAssetFiles(), assetsRead(ASSETS_CHECK::TO_CHECK)
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

	GetAllAssetFiles();

	uint toLoad = ImportResourceFromAssets("Assets/Models/Street environment_V01.FBX");
	RequestResource(toLoad);

	updateAssetsTimer.Start();

	return true;
}

updateStatus M_Resources::Update(float dt)
{
	if (updateAssetsTimer.ReadSec() > 4 && assetsRead == ASSETS_CHECK::TO_CHECK)
	{
		GetAllAssetFiles();
		assetsRead = ASSETS_CHECK::TO_IMPORT;
	}

	if (updateAssetsTimer.ReadSec() > 4.5 && assetsRead == ASSETS_CHECK::TO_IMPORT)
	{
		CheckAssetsImport(allAssetFiles);
		assetsRead = ASSETS_CHECK::TO_CHANGE;
		allAssetFiles.Clear();
	}

	if (updateAssetsTimer.ReadSec() > 5 && assetsRead == ASSETS_CHECK::TO_CHANGE)
	{
		if (onlineIdUpdated.size() > 0)
		{
			App->scene->ResoucesUpdated(&onlineIdUpdated);
			onlineIdUpdated.clear();
		}
		assetsRead = ASSETS_CHECK::TO_CHECK;
		updateAssetsTimer.StartFrom(0);
	}

	return UPDATE_CONTINUE;
}

updateStatus M_Resources::PreDraw(bool* drawState)
{
	allLibFiles.Clear();

	return UPDATE_CONTINUE;
}

bool M_Resources::CleanUp()
{
	//Loaders CleanUp----------------------------
	MeshLoader::CleanUp();
	TextureLoader::CleanUp();

	return true;
}

uint M_Resources::ImportResourceFromAssets(const char* path)
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

	std::string metaFile = relativePath;
	if (!App->physFS->IsMeta(relativePath))
		metaFile += ".meta";

	if (!App->physFS->Exists(metaFile.c_str()))
	{
		return ImportResource(relativePath);
	}
	else
	{
		uint32 id = 0, date = 0;
		std::string path = "";
		GetInfoFromMeta(metaFile.c_str(), &id, &date, nullptr, &path);

		//Get the assets path
		std::string assetsPath = "";
		if (App->physFS->IsMeta(relativePath))
			assetsPath = relativePath.substr(0, relativePath.length() - 5);
		else
			assetsPath = relativePath;


		if (!App->physFS->Exists(assetsPath.c_str())) //Assets has been removed!
		{
			DeleteResource(id);
			App->physFS->DeleteFileIn(path.c_str());
			App->physFS->DeleteFileIn(metaFile.c_str());
			return 0;
		}

		//Date changed, we need to update the lib file
		if (date != App->physFS->GetCurrDate(assetsPath.c_str()))
		{
			//If the asset is in mem, we re-import & load
			// TODO: We need to find a way to "alert" components that are using that resource that has updated
			std::map<uint32, Resource*>::iterator it = resourceMap.find(id);
			if (it != resourceMap.end())
			{
				ImportResource(assetsPath, id);
				LoadResource(id);
				onlineIdUpdated.push_back(id);
			}
			else // If not, simple re-import
			{
				ImportResource(assetsPath, id);
			}
		}
		else // If nothing has changed, simple return the id
		{
			//If lib file doesn't exist, we try to re-import
			if (!App->physFS->Exists(path.c_str()))
			{
				ImportResource(assetsPath, id);
			}

		}
		return id;
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
	if (allLibFiles.IsEmpty())
	{
		std::vector<std::string> ignore_ext;
		ignore_ext.push_back(".meta");
		allLibFiles = App->physFS->GetAllFiles(LIBRARY_PATH, nullptr, &ignore_ext);
	}

	std::string foundId = App->physFS->FindInPathNode(std::to_string(id).c_str(), allLibFiles);

	if (foundId == "")
	{
		//std::vector<std::string> searchFor;
		//searchFor.push_back(".meta");
		//PathNode allAssets = App->physFS->GetAllFiles(ASSETS_FOLDER, &searchFor, nullptr);
		//TODO: Search in Assets .neta & try to find the id

		RELEASE(resource);
		return nullptr;
	}
	else
	{
		resource = LoadResource(id, foundId.c_str());
	}

	return resource;
}

Resource* M_Resources::LoadResource(uint id, const char* libPath)
{
	Resource* ret = nullptr;
	std::string libPathStr;

	if (libPath == nullptr)
	{
		std::string foundId = FindLibraryFile(id);
		if (foundId != "")
			libPathStr = foundId.c_str();
		else
			LOG("Could not find resource with id %u, load cancelled!", id);

	}
	else
		libPathStr = libPath;

	char* buffer = nullptr;
	uint size = App->physFS->Load(libPathStr.c_str(), &buffer);
	if (size == 0)
	{
		return ret;
	}

	std::string extension;
	App->physFS->SplitFilePath(libPathStr.c_str(), nullptr, nullptr, nullptr, &extension);

	R_TYPE resourceType = App->physFS->GetTypeFromExtension(extension.c_str());

	switch (resourceType)
	{
	case (R_TYPE::MESH):
	{
		ret = CreateResource(R_TYPE::MESH, id);
		MeshLoader::Private::LoadMesh((R_Mesh*)ret, &buffer, id);
		break;
	}
	case (R_TYPE::TEXTURE):
	{
		ret = CreateResource(R_TYPE::TEXTURE, id);
		R_Texture* tmp = (R_Texture*)ret;
		tmp->textureId = TextureLoader::Load(buffer, size, (R_Texture*)ret);
		break;
	}
	case (R_TYPE::MATERIAL):
	{
		ret = CreateResource(R_TYPE::MATERIAL, id);
		MaterialLoader::Load((R_Material*)ret, buffer);
		break;
	}
	case (R_TYPE::MODEL):
	{
		ret = new R_Model(id);
		ModelLoader::Load((R_Model*)ret, buffer);
		RELEASE(ret);
		break;
	}
	case (R_TYPE::SCENE):
	{
		ret = new R_Scene(id);

		deleteResources = false;
		App->scene->ResetScene();
		SceneLoader::Load(buffer);

		deleteResources = true;
		CheckResourcesToUnload();
		break;
	}
	}

	RELEASE_ARRAY(buffer);

	if (ret != nullptr)
		ret->referenceCount++;

	return ret;
}

Resource* M_Resources::CreateResource(R_TYPE rType, uint32 forceId)
{
	Resource* ret = nullptr;

	//TODO: Check if the resource already exists, if it does, get his id & delete the old one
	//TODO 2: Also, instances :)
	uint32 uId = (forceId != 0 ? forceId : GenerateId());

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

uint M_Resources::SaveResource(Resource* toSave, std::string assetsPath, bool saveMeta)
{
	char* buffer = nullptr;
	uint size = 0;
	std::string libraryPath;

	switch (toSave->GetType())
	{
	case R_TYPE::MODEL:
	{
		R_Model* myModel = (R_Model*)toSave;
		size = ModelLoader::Save(myModel, &buffer);
		libraryPath = myModel->GetLibraryPath();
	}
	break;
	case R_TYPE::SCENE:
	{
		R_Scene* myScene = (R_Scene*)toSave;
		size = SceneLoader::Save(myScene->root, &buffer);
		libraryPath = myScene->GetLibraryPath();
	}
	break;
	case R_TYPE::MESH:
	{
		R_Mesh* myMesh = (R_Mesh*)toSave;
		size = MeshLoader::Save(myMesh, &buffer);
		libraryPath = myMesh->GetLibraryPath();
	}
	break;
	case R_TYPE::TEXTURE:
	{
		R_Texture* myTexture = (R_Texture*)toSave;
		size = TextureLoader::Save(&buffer);
		libraryPath = myTexture->GetLibraryPath();
	}
	break;
	case R_TYPE::MATERIAL:
	{
		R_Material* myMaterial = (R_Material*)toSave;
		size = MaterialLoader::Save(myMaterial, &buffer);
		libraryPath = myMaterial->GetLibraryPath();
	}
	break;
	}

	if (size > 0)
	{
		allLibFiles.Clear();

		App->physFS->Save(libraryPath.c_str(), buffer, size);

		if (saveMeta == true)
			SaveMeta(toSave, assetsPath);

		RELEASE_ARRAY(buffer);

		return toSave->GetUid();
	}

	return 0;
}

bool M_Resources::SaveMeta(Resource* toSave, std::string assetsPath)
{
	JsonConfig config;

	config.AddNumber("ID", toSave->GetUid());
	config.AddString("Name", toSave->GetName().c_str());
	config.AddNumber("Type", (int)toSave->GetType());

	//TODO: This maybe isn't necessary (?)
	config.AddString("LibraryFile", toSave->GetLibraryPath().c_str());

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

bool M_Resources::DeleteResource(uint32 idToDestroy)
{
	bool ret = false;

	if (deleteResources)
	{
		std::map<uint, Resource*>::iterator it = resourceMap.find(idToDestroy);
		if (it != resourceMap.end())
		{
			RELEASE(it->second);
			resourceMap.erase(it);
			ret = true;
		}
	}
	else
		ret = true;

	return ret;
}

void M_Resources::UnloadResource(Resource* toUnload)
{
	toUnload->referenceCount--;
	if (toUnload->referenceCount <= 0)
	{
		DeleteResource(toUnload->GetUid());
	}
}

void M_Resources::UnloadResource(uint32 toUnloadId)
{
	std::map<uint, Resource*>::iterator it = resourceMap.find(toUnloadId);
	if (it != resourceMap.end())
	{
		it->second->referenceCount--;
		if (it->second->referenceCount <= 0)
		{
			DeleteResource(toUnloadId);
		}
	}
}

void M_Resources::CheckResourcesToUnload()
{
	std::map<uint, Resource*>::const_iterator it = resourceMap.begin();

	int size = resourceMap.size();

	for (int i = 0; i < size; i++)
	{
		if (it->second->referenceCount <= 0)
		{
			DeleteResource(it->first);
			it = resourceMap.begin();
			i = 0;
			size = resourceMap.size();
			continue;
		}

		it++;
	}
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

bool M_Resources::GetInfoFromMeta(const char* metaPath, uint32* id, uint32* modDate, R_TYPE* type, std::string* path)
{
	if (id == nullptr && modDate == nullptr && type == nullptr && path == nullptr)
	{
		return false;
	}

	char* buffer = nullptr;
	uint size = App->physFS->Load(metaPath, &buffer);
	if (size > 0)
	{
		JsonConfig metaData(buffer);

		if (metaData.IsInitialized())
		{
			if (id != nullptr)
			{
				*id = metaData.GetNumber("ID");
			}
			if (modDate != nullptr)
			{
				*modDate = metaData.GetNumber("Date");
			}
			if (type != nullptr)
			{
				*type = (R_TYPE)(int)metaData.GetNumber("Type");
			}
			if (path != nullptr)
			{
				*path = metaData.GetString("LibraryFile");
			}
			RELEASE(buffer);

		}
		else
		{
			LOG("Could not open the config from meta file %s ", metaPath);
			RELEASE(buffer);
			return false;
		}
	}
	else
	{
		LOG("Could not load meta file %s; did not exist or it's corrupted", metaPath);
		return false;
	}

	return true;
}

uint32 M_Resources::GenerateId()
{
	return randomGen.Int();
}

std::string M_Resources::FindLibraryFile(uint id)
{
	if (allLibFiles.IsEmpty())
		allLibFiles = App->physFS->GetAllFiles(LIBRARY_PATH, nullptr, nullptr);

	std::string foundId = App->physFS->FindInPathNode(std::to_string(id).c_str(), allLibFiles);

	return foundId;
}

void M_Resources::SetDeleteResources(bool newState)
{
	if (newState != deleteResources)
		deleteResources = !deleteResources;
}

void M_Resources::GetResourcesByType(std::vector<const Resource*>& resources, R_TYPE type) const
{
	for (std::map<uint, Resource*>::const_iterator it = resourceMap.begin(); it != resourceMap.end(); ++it)
	{
		if (it->second->GetType() == type)
			resources.push_back(it->second);
	}
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

uint32 M_Resources::ImportResource(std::string path, uint32 forceid)
{
	R_TYPE resourceType = GetResourceTypeFromExtension(path.c_str());

	if (resourceType != R_TYPE::NO_TYPE)
	{
		Resource* newResource = CreateResource(resourceType, forceid);

		char* fileBuffer = nullptr;
		uint64 fileSize = 0;
		fileSize = App->physFS->Load(path.c_str(), &fileBuffer);

		switch (resourceType)
		{
		case (R_TYPE::TEXTURE):				TextureLoader::Import(path.c_str(), &fileBuffer, fileSize); break;
		case (R_TYPE::MODEL):				ImportModel(path.c_str(), &fileBuffer, fileSize, (R_Model*)newResource); break;
		}

		SaveResource(newResource, path);

		uint newResourceID = newResource->GetUid();
		RELEASE_ARRAY(fileBuffer);
		DeleteResource(newResource->GetUid());

		return newResourceID;
	}
	else
	{
		LOG("Could not import %s! Extension unknown or file not found", path.c_str());
	}
}

void M_Resources::ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel)
{
	const aiScene* scene = aiImportFileFromMemory(*buffer, bufferSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene)
	{
		//WARNING: assuming that all the mesh is made from triangles
		std::string fbxName = "";
		App->physFS->SplitFilePath(path, nullptr, nullptr, &fbxName, nullptr);
		uint32 ID = resourceModel->AddModelNode(GenerateId(), fbxName.c_str(), float4x4::identity, App->scene->GetRoot()->GetUid());

		MeshLoader::Private::LoadAiSceneMeshes(scene, scene->mRootNode, path, resourceModel, ID);

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading aiScene %s", path);
	}
}

void M_Resources::GetAllAssetFiles()
{
	static std::vector<std::string> ignoreExt = { "meta" };
	allAssetFiles = App->physFS->GetAllFiles(ASSETS_FOLDER, nullptr, &ignoreExt);
}

void M_Resources::CheckAssetsImport(PathNode& pathnode)
{
	if (pathnode.isFile)
	{
		ImportResourceFromAssets(pathnode.path.c_str());
	}

	if (!pathnode.isLeaf)
	{
		for (int i = 0; i < pathnode.children.size(); i++)
		{
			CheckAssetsImport(pathnode.children[i]);
		}
	}
}
