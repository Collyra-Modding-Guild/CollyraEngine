#pragma once

#include <string>
#include <vector>
#include <string>

enum class COMPONENT_TYPE
{
	NO_TYPE = -1,
	TRANSFORM,
	MESH,
	MATERIAL
};

class Component;

class GameObject
{
public:

	GameObject(std::string name);
	virtual ~GameObject();

	void Start();
	void Update(float dt);

	Component* CreateComponent(COMPONENT_TYPE type);

	//If succesfull to Add return the component, else return nullptr
	Component* AddComponent(Component* c);

	template<typename T>
	T* GetComponent();

	unsigned int GetId() const;
	void SetId(unsigned int newId);

	std::string GetName() const;

	void SetName(std::string newName);

public:
	std::vector<Component*> components;
	GameObject* parent;
	std::vector<GameObject*> children;

	bool active;

private:
	std::string name;
	unsigned int id;

};

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
