#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>
#include <stack>

class GameObject;
class Component;

class M_Scene : public Module
{
public:

	M_Scene(MODULE_TYPE type, bool startEnabled = true);
	~M_Scene();

	bool Awake() override;
	updateStatus Update(float dt) override;
	bool CleanUp() override;

	GameObject* CreateGameObject(GameObject* parent);
	bool DeleteGameObject(unsigned int id);
	bool DeleteGameObject(GameObject* gameObject);

	unsigned int GenerateId();

private:
	GameObject* root;
	unsigned int globalId;

};

#endif // __ModuleResources_H__

