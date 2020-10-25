#pragma once

#include "GameObject.h"

class Component
{
public:

	Component(COMPONENT_TYPE type,  bool active = true);
	virtual ~Component();

	virtual void Start();
	virtual void Update(float dt);

	virtual void Enable();
	virtual void Disable();

	GameObject* GetGameObject()	const;
	void	SetGameObject(GameObject* go);


public:
	COMPONENT_TYPE type;
	bool active;

private:
	GameObject* myGameObject;

};
