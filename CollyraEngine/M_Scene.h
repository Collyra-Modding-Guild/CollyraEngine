#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>
#include <stack>
#include <string>

class GameObject;
class Component;

class M_Scene : public Module
{
public:

	M_Scene(MODULE_TYPE type, bool startEnabled = true);
	~M_Scene();

	bool Awake() override;
	updateStatus Update(float dt) override;
	updateStatus Draw(float dt);
	updateStatus DebugDraw(float dt);

	bool CleanUp() override;

	GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr);
	bool DeleteGameObject(unsigned int id);
	bool DeleteGameObject(GameObject* gameObject);

	unsigned int GenerateId();

	const GameObject* GetRoot();

private:
	void CheckSiblingsName(GameObject* parent, std::string& myName);

private:
	GameObject* root;
	unsigned int globalId;

};

#endif // __ModuleResources_H__

