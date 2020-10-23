#include "Component.h"

Component::Component() : active(true), type(COMPONENT_TYPE::NO_TYPE), myGameObject(nullptr)
{
}

Component::~Component()
{
}

void Component::Update()
{
}

void Component::Enable()
{
}

void Component::Disable()
{
}
