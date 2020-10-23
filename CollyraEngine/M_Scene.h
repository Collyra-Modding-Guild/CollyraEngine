#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>

class GameObject;

class M_Scene : public Module
{
public:

	M_Scene(MODULE_TYPE type, bool startEnabled = true);
	~M_Scene();

	bool Awake() override;
	updateStatus Update(float dt) override;
	bool CleanUp() override;

	GameObject* CreateGameObject();

public:
	GameObject* root;

};

#endif // __ModuleResources_H__
