#pragma once

#include <string>
#include <vector>

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

	GameObject();
	virtual ~GameObject();

	void Start();
	void Update(float dt);

	Component* CreateComponent(COMPONENT_TYPE type);
	void AddComponent(Component* c);

	template<typename T>
	T* GetComponent();

	unsigned int GetId() const;
	void SetId(unsigned int newId);

public:
	std::vector<Component*> components;
	GameObject* parent;
	std::vector<GameObject*> children;

private:
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
