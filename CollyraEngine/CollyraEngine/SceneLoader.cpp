#include "SceneLoader.h"
#include "M_Scene.h"
#include "GameObject.h"
#include "M_FileManager.h"
#include "Application.h"

#include "Component.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"

#include "JsonConfig.h"

#include "MeshLoader.h"
#include "MaterialLoader.h"

uint SceneLoader::Save(const GameObject* hierarchyParent, char** buffer)
{
	JsonConfig jsonFile;
	jsonFile.AddString("SceneName", hierarchyParent->GetName().c_str());
	JsonArray gameObjectsArr = jsonFile.CreateElementArray("GameObjects");

	const std::vector<GameObject*> gameObjects = hierarchyParent->GetChilds();

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		Private::SaveGameObject(gameObjects[i], gameObjectsArr.AddValue(), &gameObjectsArr);
	}

	uint size = jsonFile.Serialize(buffer);
	jsonFile.ReleaseFile();
	return size;
}

bool SceneLoader::Load(char* buffer)
{
	JsonConfig jsonFile(buffer);
	if (jsonFile.IsInitialized())
	{
		App->scene->SetSceneName(jsonFile.GetString("SceneName").c_str());
		JsonArray gameObjectsArr = jsonFile.GetArray("GameObjects");
		if (gameObjectsArr.IsInitialized())
		{
			Private::LoadGameObjects(gameObjectsArr);
		}
	}

	return true;
}

void SceneLoader::Private::SaveGameObject(const GameObject* gameObject, JsonConfig saveTo, JsonArray* gameObjectsArray)
{
	saveTo.AddNumber("UID", gameObject->GetUid());
	saveTo.AddString("Name", gameObject->GetName().c_str());
	saveTo.AddBool("Active", gameObject->active);

	saveTo.AddNumber("ParentUID", gameObject->GetParent()->GetUid());

	//TODO: Save hierarchy open + Selected (???)

	JsonArray compConfig = saveTo.CreateElementArray("Components");
	const std::vector<Component*>* components = gameObject->GetAllComponents();

	for (uint i = 0; i < components->size(); i++)
	{
		SaveComponent(components->at(i), compConfig.AddValue());
	}

	//Save Children Recursive----
	for (uint i = 0; i < gameObject->children.size(); ++i)
	{
		Private::SaveGameObject(gameObject->children[i], gameObjectsArray->AddValue(), gameObjectsArray);
	}
}

void SceneLoader::Private::SaveComponent(const Component* component, JsonConfig saveTo)
{
	saveTo.AddNumber("Type", (int)component->GetType());
	saveTo.AddBool("Active", component->IsActive());

	//Private::SaveComponent(saveTo, component);
	//TODO: Obv, all of these (except transform) have to be resources & just save the id
	//of the resource
	switch (component->GetType())
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		C_Transform* compToSave = (C_Transform*)component;

		saveTo.CreateElementArray("Translation").AddFloat3(compToSave->GetPosition());
		saveTo.CreateElementArray("Rotation").AddQuat(compToSave->GetRotation());
		saveTo.CreateElementArray("Scale").AddFloat3(compToSave->GetScale());
	}
	break;
	case COMPONENT_TYPE::MESH:
	{
		C_Mesh* compToSave = (C_Mesh*)component;

		//TODO: Save ID
		saveTo.AddNumber("MeshID", compToSave->GetResourceId());
	}
	break;
	case COMPONENT_TYPE::MATERIAL:
	{
		C_Material* compToSave = (C_Material*)component;

		saveTo.AddNumber("MaterialID", compToSave->GetResourceId());
	}
	break;
	case COMPONENT_TYPE::CAMERA:
	{
		C_Camera* compToSave = (C_Camera*)component;

		saveTo.AddNumber("HorizontalFov", compToSave->GetHorizontalFov());
		saveTo.AddNumber("Near Plane", compToSave->GetNearPlane());
		saveTo.AddNumber("Far Plane", compToSave->GetFarPlane());
	}
	break;
	default:
	{
		LOG("Error saving component! Type not specified!")
	}
	break;
	}
}

void SceneLoader::Private::LoadGameObjects(JsonArray& gameObjectsArr)
{
	for (uint i = 0; i < gameObjectsArr.Size(); i++)
	{
		JsonConfig gameObjectInfo = gameObjectsArr.GetNode(i);

		if (gameObjectInfo.IsInitialized())
		{
			GameObject* newGameObject = new GameObject(gameObjectInfo.GetString("Name").c_str());
			newGameObject->SetUid(gameObjectInfo.GetNumber("UID"));

			newGameObject->SetActive(gameObjectInfo.GetBool("Active"));
			newGameObject->SetParent(App->scene->GetGameObject(gameObjectInfo.GetNumber("ParentUID")));

			JsonArray components = gameObjectInfo.GetArray("Components");

			if (components.IsInitialized())
			{
				LoadComponents(components, newGameObject);
			}
			else
			{
				LOG("Error in scene load! Could not load Components of %s GameObject!", newGameObject->GetName().c_str());
			}
		}
		else
		{
			LOG("Error in scene load! Could not load GameObject in index %i", i);
		}

	}
}

void SceneLoader::Private::LoadComponents(JsonArray& components, GameObject* gameObject)
{
	for (uint i = 0; i < components.Size(); i++)
	{
		JsonConfig componentInfo = components.GetNode(i);
		if (componentInfo.IsInitialized())
		{
			COMPONENT_TYPE cmpType = (COMPONENT_TYPE)((int)componentInfo.GetNumber("Type"));

			if (Component* component = gameObject->CreateComponent(cmpType))
			{
				//TODO: This load should be a resource requested to Resource Manager
				//As with everything, is resource is loaded in mem, just get the id
				//If it is not, try loading it from library, if not, try loading it from assets
				//And if not, why do you mess with the file structure, do you want to create a game or not?

				Private::LoadComponent(componentInfo, component);
			}
			else
			{
				LOG("Could not create component in %s! Type was %i", gameObject->GetName().c_str(), (int)cmpType);
			}
		}
		else
		{
			LOG("Error loading components in %s! Could not load from JSON!", gameObject->GetName().c_str());
		}

	}
}

void SceneLoader::Private::LoadComponent(JsonConfig& componentInfo, Component* component)
{
	switch (component->GetType())
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		C_Transform* compToSave = (C_Transform*)component;
		float3 translate = componentInfo.GetArray("Translation").GetFloat3();
		Quat rot = componentInfo.GetArray("Rotation").GetQuaternion();
		float3 scale = componentInfo.GetArray("Scale").GetFloat3();

		compToSave->SetLocalTransformation(translate, rot, scale);
		compToSave->GenerateGlobalTransformationFrom(compToSave->GetGameObject()->GetParent()->GetComponent<C_Transform>()->GetGlobalTransform());
		compToSave->hasUpdated = true;

	}
	break;
	case COMPONENT_TYPE::MESH:
	{
		C_Mesh* compToSave = (C_Mesh*)component;

		compToSave->SetResourceId(componentInfo.GetNumber("MeshID"));
	}
	break;
	case COMPONENT_TYPE::MATERIAL:
	{
		C_Material* compToSave = (C_Material*)component;

		compToSave->SetResourceId(componentInfo.GetNumber("MaterialID"));
	}
	break;
	case COMPONENT_TYPE::CAMERA:
	{
		C_Camera* compToSave = (C_Camera*)component;

		compToSave->SetHorizontalFov(componentInfo.GetNumber("HorizontalFov"));
		compToSave->SetNearPlane(componentInfo.GetNumber("Near Plane"));
		compToSave->SetFarPlane(componentInfo.GetNumber("Far Plane"));
	}
	break;
	default:
	{
		LOG("Error saving component! Type not specified!")
	}
	break;
	}
}

