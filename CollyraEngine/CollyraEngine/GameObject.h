#pragma once

#include "MathGeoLib/include/MathGeoLib.h"

#include <string>
#include <vector>
#include <map>
#include "p2Defs.h"

enum class COMPONENT_TYPE
{
	NO_TYPE = -1,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	SCRIPT,

	MAX_TYPES
};

class Component;

class GameObject
{
public:

	GameObject(std::string name);
	virtual ~GameObject();

	void Start();
	void Update(float dt);
	void PostUpdate(float dt);

	Component* CreateComponent(COMPONENT_TYPE type);

	//If succesfull to Add return the component, else return nullptr
	Component* AddComponent(Component* c);

	bool DeleteComponent(Component* toDelete);

	template<typename T>
	T* GetComponent();
	template<typename T>
	std::vector<T*> GetComponentList();
	const std::vector<Component*>* GetAllComponents() const;

	uint32 GetUid() const;
	void SetUid(uint32 newId);

	bool IsSelected() const;
	void SetSelected(bool newId);

	std::string GetName() const;
	void SetName(std::string newName);

	void SetActive(bool newState);

	void SetParent(GameObject* newParent);
	GameObject* GetParent() const;

	void NotifyChildDeath(GameObject* deadChild);
	void AddChildren(GameObject* newChild);
	GameObject* GetChild(int id) const;
	std::vector<GameObject*> GetChilds() const;

	const AABB GetGameObjectAABB() const;	

	void ResourcesUpdated(std::map<unsigned int, bool>* resourcesUpdated);

private:

	void BoundingBoxUpdate();


public:
	std::vector<Component*> components;
	std::vector<GameObject*> children;

	bool active;

private:
	std::string name;
	GameObject* parent;
	uint32 uid;

	AABB aabb;
	OBB	obb;

	bool selected;

};

//Unity Style Yeah----------------
template<typename T>
T* GameObject::GetComponent()
{
	for (int i = 0; i < components.size(); i++)
	{
		T* c = dynamic_cast<T*>(components[i]);
		if (c != nullptr)
			return	c;
	}
	return nullptr;
}

template<typename T>
std::vector<T*> GameObject::GetComponentList()
{
	std::vector<T*> componentVector;
	for (int i = 0; i < components.size(); i++)
	{
		T* c = dynamic_cast<T*>(components[i]);
		if (c != nullptr)
			componentVector.push_back(c);
	}

	return componentVector;
}
