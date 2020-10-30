#include "Component.h"

Component::Component(COMPONENT_TYPE type, bool active) :
	active(active), type(type), myGameObject(nullptr)
{}

Component::~Component()
{
	myGameObject = nullptr;
}

void Component::Start()
{}

void Component::Update(float dt)
{}

void Component::Enable()
{}

void Component::Disable()
{}

bool Component::IsActive() const
{
	return active;
}

void Component::SetActive(bool newState)
{
	if (newState != active)
		active = !active;
}

GameObject* Component::GetGameObject() const
{
	return	myGameObject;
}

void Component::SetGameObject(GameObject* go)
{
	myGameObject = go;
}

COMPONENT_TYPE Component::GetType() const
{
	return type;
}
