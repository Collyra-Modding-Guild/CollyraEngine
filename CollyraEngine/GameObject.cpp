#include "GameObject.h"

GameObject::GameObject() : parent(nullptr)
{}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	return nullptr;
}
