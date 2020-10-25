#include "GameObject.h"
#include "Component.h"
#include "p2Defs.h"

#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"

GameObject::GameObject() : parent(nullptr), id(0)
{}

GameObject::~GameObject()
{
	for (int i = 0; i < children.size(); i++)
	{
		RELEASE(children[i]);
	}

	children.clear();
	parent = nullptr;

	for (int i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}
	components.clear();
}

void GameObject::Start()
{
	for (auto c : components)
		c->Start();
}

void GameObject::Update(float dt)
{
	for (auto c : components)
		c->Update(dt);
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* newComponent = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		newComponent = new C_Transform();
	}
		break;
	case COMPONENT_TYPE::MESH:
	{
		newComponent = new C_Mesh();
	}
		break;
	case COMPONENT_TYPE::MATERIAL:
	{
		newComponent = new C_Mesh();
	}
		break;
	default:
		break;
	}

	if (newComponent != nullptr)
	{
		AddComponent(newComponent);
	}

	return newComponent;
}

unsigned int GameObject::GetId() const
{
	return id;
}

void GameObject::SetId(unsigned int newId)
{
	this->id = newId;
}

void GameObject::AddComponent(Component* c)
{
	c->SetGameObject(this);
	components.push_back(c);
}