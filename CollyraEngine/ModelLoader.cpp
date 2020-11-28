#include "ModelLoader.h"

#include "R_Model.h"
#include "M_Scene.h"
#include "Application.h"
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Material.h"

#include "JsonConfig.h"
#include "MathGeoLib/include/MathGeoLib.h"

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

				uint32 meshID = modelNode.GetNumber("MeshID");
				if (modelNode.GetNumber("MeshID") != -1)
				{
					C_Mesh* meshComponent = (C_Mesh*)newGameObject->CreateComponent(COMPONENT_TYPE::MESH);
					//TODO: Set Resource should call the ResourceManager (Give me my Resource >:( )
					meshComponent->SetResourceId(meshID);
				}

				uint32 materialID = modelNode.GetNumber("MaterialID");
				if (modelNode.GetNumber("MaterialID") != -1)
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
