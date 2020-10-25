#include "Component.h"

Component::Component(COMPONENT_TYPE type, bool active) :
	active(active), type(type), myGameObject(nullptr)
{}

Component::~Component()
{}

void Component::Start()
{}

void Component::Update(float dt)
{}

void Component::Enable()
{}

void Component::Disable()
{}

GameObject* Component::GetGameObject() const
{
	return	myGameObject;
}

void Component::SetGameObject(GameObject* go)
{
	myGameObject = go;
}
