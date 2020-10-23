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

	void Update();

	Component* CreateComponent(COMPONENT_TYPE type);


public:
	std::string name;
	std::vector<Component*> components;
	GameObject* parent;
	std::vector<GameObject*> children;

};
