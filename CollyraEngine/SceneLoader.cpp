#include "SceneLoader.h"
#include "M_Scene.h"
#include "GameObject.h"

#include "Component.h"
#include "C_Camera.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"

#include "JsonConfig.h"

uint SceneLoader::Save(const GameObject* hierarchyParent, char** buffer)
{
	JsonConfig jsonFile;
	JsonArray gameObjectsArr = jsonFile.CreateElementArray("GameObjects");

	const std::vector<GameObject*> gameObjects = hierarchyParent->GetChilds();

	//If we're not saving the scene, we include the hierarchy parent
	if (hierarchyParent->GetUid() != ROOT_ID)
	{
		Private::SaveGameObject(hierarchyParent, gameObjectsArr.AddValue(), &gameObjectsArr);

	}

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		Private::SaveGameObject(gameObjects[i], gameObjectsArr.AddValue(), &gameObjectsArr);
	}

	uint size = jsonFile.Serialize(buffer);
	jsonFile.ReleaseFile();
	return size;
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
		SaveComponent(components->at(i),compConfig.AddValue());
	}

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

		saveTo.AddString("Name", compToSave->GetMeshName().c_str());
		saveTo.AddString("Path", compToSave->GetMeshPath().c_str());

	}
		break;
	case COMPONENT_TYPE::MATERIAL:
	{
		C_Material* compToSave = (C_Material*)component;

		saveTo.AddString("MatName", compToSave->GetMaterialName().c_str());
		saveTo.AddString("MatPath", compToSave->GetMaterialPath().c_str());

		saveTo.AddString("TextName", compToSave->GetTextureName().c_str());
		saveTo.AddString("TextPath", compToSave->GetTexturePath().c_str());
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

