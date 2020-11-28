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

	bool IsActive() const;
	void SetActive(bool newState);

	GameObject* GetGameObject()	const;
	void	SetGameObject(GameObject* go);

	COMPONENT_TYPE GetType() const;

	virtual void ResourceUpdated(std::vector<unsigned int>* ids);

protected:
	COMPONENT_TYPE type;
	bool active;
	GameObject* myGameObject;

};
