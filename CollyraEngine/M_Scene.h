#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>
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
	updateStatus PostUpdate(float dt) override;
	updateStatus Draw(bool* drawState);

	bool CleanUp() override;

	GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr);
	bool		DeleteGameObject(unsigned int id);
	GameObject* GetGameObject(unsigned int id);
	GameObject* GetGameObject(std::string name);
	bool		DeleteGameObject(GameObject* gameObject);

	unsigned int GenerateId();

	GameObject* GetRoot();	

private:
	void CheckSiblingsName(GameObject* parent, std::string& myName);
	void DrawGameObject(GameObject* gameObject, bool* drawState);
	void DrawBoundingBox(GameObject* gameObject);

public:
	GameObject* focusedGameObject;

private:
	GameObject* root;
	unsigned int globalId;

};

#endif // __ModuleResources_H__

