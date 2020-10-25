#include "GameObject.h"
#include "Component.h"
#include "p2Defs.h"

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
	return nullptr;
}

unsigned int GameObject::GetId() const
{
	return id;
}

void GameObject::SetId(unsigned int newId)
{
	this->id = newId;
}

template<typename T>
T* GameObject::GetComponent()
{
	for (int i = 0; i < components.size(); i++)
	{
		T* c = dynamic_cast<T*>components[i];
		if (c != nullptr)
			return	c;
	}
	return nullptr;
}

void GameObject::AddComponent(Component* c)
{
	c->SetGameObject(this);
	components.push_back(c);
}