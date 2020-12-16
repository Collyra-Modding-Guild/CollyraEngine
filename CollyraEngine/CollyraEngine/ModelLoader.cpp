#include "ModelLoader.h"

#include "R_Model.h"
#include "M_Scene.h"
#include "M_FileManager.h"
#include "Application.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "MeshLoader.h"
#include "M_Resources.h"
#include "R_Mesh.h"
#include "MaterialLoader.h"

#include "JsonConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "Assimp/include/mesh.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

void ModelLoader::ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel)
{
	//WARNING: assuming that all the mesh is made from triangles
	const aiScene* scene = aiImportFileFromMemory(*buffer, bufferSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene)
	{
		std::string fbxName = "";
		App->physFS->SplitFilePath(path, nullptr, nullptr, &fbxName, nullptr);
		uint32 ID = resourceModel->AddModelNode(App->scene->GenerateId(), fbxName.c_str(), float4x4::identity, App->scene->GetRoot()->GetUid());

		std::vector<uint> meshes, materials;
		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			std::string meshName;
			for (uint n = 0; n < resourceModel->modelNodes.size(); ++n)
			{
				if (resourceModel->modelNodes[n].meshID == i)
					meshName = resourceModel->modelNodes[n].name;
			}
			meshes.push_back(MeshLoader::ImportMeshFromModel(path, scene->mMeshes[i], i));
		}
		for (uint i = 0; i < scene->mNumMaterials; ++i)
		{
			aiString matName;
			scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);

			materials.push_back(MaterialLoader::ImportMaterialFromMesh(path, scene->mMaterials[i], matName.C_Str()));
		}

		ModelLoader::Private::LoadAiSceneMeshes(scene, scene->mRootNode, path, resourceModel, ID);
		Private::LinkModelResources(resourceModel, meshes, materials);

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading aiScene %s", path);
	}
}

bool ModelLoader::Private::LoadAiSceneMeshes(const aiScene* scene, const aiNode* parent, const char* filePath, R_Model* myResource, unsigned int parentID, float4x4 transform)
{
	for (int i = 0; i < parent->mNumChildren; i++)
	{
		LoadNodeMeshes(scene, parent->mChildren[i], filePath, myResource, parentID, transform);
	}

	return true;
}

void ModelLoader::Private::LinkModelResources(R_Model* model, const std::vector<uint>& meshes, const std::vector<uint>& materials)
{
	for (uint i = 0u; i < model->modelNodes.size(); ++i)
	{
		if (model->modelNodes[i].meshID != -1)
		{
			model->modelNodes[i].meshID = meshes[model->modelNodes[i].meshID];
			if(model->modelNodes[i].meshID == 0)
				model->modelNodes[i].meshID = -1;

		}
		else
			model->modelNodes[i].meshID = -1;


		if (model->modelNodes[i].materialID != -1)
		{
			model->modelNodes[i].materialID = materials[model->modelNodes[i].materialID];
			if (model->modelNodes[i].materialID == 0)
				model->modelNodes[i].materialID = -1;
		}
		else
			model->modelNodes[i].materialID = -1;
	}
}


bool ModelLoader::Private::LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, R_Model* myResource, unsigned int panretId, float4x4 transform)
{
	unsigned int myID = panretId;

	aiVector3D translation, scaling;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	float3 scale(scaling.x, scaling.y, scaling.z);

	transform = transform * float4x4::FromTRS(pos, rot, scale);

	if (node->mNumMeshes > 0)
	{
		int meshId = -1;
		int materialId = -1;

		//Mesh Load---------
		int meshSize = node->mNumMeshes;
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			meshId = node->mMeshes[i];

			//MATERIAL

			materialId = scene->mMeshes[meshId]->mMaterialIndex;

			int myIDbuff = myResource->AddModelNode(App->resources->GenerateId(), node->mName.C_Str(), transform, panretId, meshId, materialId);

			if (i == node->mNumMeshes - 1)
			{
				transform = float4x4::identity;
			}
		}
	}

	LoadAiSceneMeshes(scene, node, filePath, myResource, myID, transform);

	return true;
}


uint ModelLoader::Save(const R_Model* rModel, char** buffer)
{
	JsonConfig jsonFile;
	JsonArray gameObjectsArr = jsonFile.CreateElementArray("ModelNodes");

	for (uint i = 0; i < rModel->modelNodes.size(); ++i)
	{
		Private::SaveGameModel(&rModel->modelNodes[i], gameObjectsArr.AddValue(), &gameObjectsArr);
	}

	uint size = jsonFile.Serialize(buffer);
	jsonFile.ReleaseFile();
	return size;

}

void ModelLoader::Load(R_Model* rModel, char* buffer)
{
	JsonConfig jsonFile(buffer);
	std::vector<GameObject*> createdGameObjects;
	if (jsonFile.IsInitialized())
	{
		JsonArray gameObjectsArr = jsonFile.GetArray("ModelNodes");
		if (gameObjectsArr.IsInitialized())
		{
			for (uint i = 0u; i < gameObjectsArr.Size(); ++i)
			{
				JsonConfig modelNode = gameObjectsArr.GetNode(i);

				GameObject* parent = App->scene->GetGameObject(modelNode.GetNumber("ParentID"));

				if (parent == nullptr)
				{
					LOG("Could not find parent for GameObject with id %i; stopping model loading...", modelNode.GetNumber("ID"));
					return;
				}

				GameObject* newGameObject = App->scene->CreateGameObject(modelNode.GetString("Name"), parent);
				createdGameObjects.push_back(newGameObject);
				newGameObject->SetUid(modelNode.GetNumber("ID"));
				float3 translate = modelNode.GetArray("Translation").GetFloat3();
				Quat rot = modelNode.GetArray("Rotation").GetQuaternion();
				float3 scale = modelNode.GetArray("Scale").GetFloat3();
				newGameObject->GetComponent<C_Transform>()->SetLocalTransformation(translate, rot, scale);

				int meshID = modelNode.GetNumber("MeshID");
				if (meshID != -1)
				{
					C_Mesh* meshComponent = (C_Mesh*)newGameObject->CreateComponent(COMPONENT_TYPE::MESH);
					meshComponent->SetResourceId(meshID);
				}

				int materialID = modelNode.GetNumber("MaterialID");
				if (materialID != -1)
				{
					C_Material* materialComponent = (C_Material*)newGameObject->CreateComponent(COMPONENT_TYPE::MATERIAL);
					materialComponent->SetResourceId(materialID);
				}
			}

			for (int i = 0; i < createdGameObjects.size(); i++)
			{
				createdGameObjects[i]->SetUid(App->scene->GenerateId());
			}
		}
	}

}

void ModelLoader::Private::SaveGameModel(const ModelNode* modelNode, JsonConfig saveTo, JsonArray* gameObjectsArray)
{
	saveTo.AddNumber("ID", modelNode->ID);
	saveTo.AddNumber("ParentID", modelNode->parentID);
	saveTo.AddString("Name", modelNode->name.c_str());

	//TODO: We should save this in the ModelNode & not have to decompose here
	float3 pos;
	Quat rot;
	float3 scale;
	modelNode->transform.Decompose(pos, rot, scale);

	saveTo.CreateElementArray("Translation").AddFloat3(pos);
	saveTo.CreateElementArray("Rotation").AddQuat(rot);
	saveTo.CreateElementArray("Scale").AddFloat3(scale);

	saveTo.AddNumber("MeshID", modelNode->meshID);
	saveTo.AddNumber("MaterialID", modelNode->materialID);
}
