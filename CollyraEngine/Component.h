#pragma once

#include "GameObject.h"

class Component
{
public:

	Component();
	~Component();

	virtual void Update();

	virtual void Enable();
	virtual void Disable();

public:
	COMPONENT_TYPE type;
	bool active;
	GameObject* myGameObject;

};
