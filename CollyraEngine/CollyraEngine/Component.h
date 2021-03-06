#pragma once

#include "GameObject.h"

class __declspec(dllexport) Component
{

public:

	Component(COMPONENT_TYPE type,  bool active = true);
	virtual ~Component();

	virtual void Start();
	virtual void Update(float dt);

	virtual void OnEnable();
	virtual void OnDisable();

	bool IsActive() const;
	void SetActive(bool newState);

	GameObject* GetGameObject()	const;
	void	SetGameObject(GameObject* go);

	COMPONENT_TYPE GetType() const;

	virtual void ResourceUpdated(std::map<unsigned int, bool>* ids);

protected:
	COMPONENT_TYPE type;
	bool active;
	GameObject* myGameObject;

};
