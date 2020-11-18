#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>
#include <string>
#include "p2Defs.h"
#include "MathGeoLib/include/Algorithm/Random/LCG.h"

#define ROOT_ID 0
#define DEFAULT_SCENE_NAME "DefaultScene"

class GameObject;
class Component;
class C_Camera;

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
	void		DeleteCamera(Component* component);

	//Delete all GameObjects, Components, cameras, reset Name,....
	void		ResetScene();

	uint32 GenerateId();

	GameObject* GetRoot();	

	std::string GetSceneName() const;
	void SetSceneName(const char* newName);

private:
	void CheckSiblingsName(GameObject* parent, std::string& myName);
	void DrawGameObject(GameObject* gameObject, bool* drawState);
	void DrawBoundingBox(GameObject* gameObject);
	void DrawFrustum(GameObject* gameObject);

public:
	GameObject* focusedGameObject;
	std::vector<C_Camera*> cameras;
	LCG randomGenerator;

private:
	GameObject* root;
	std::string sceneName;

};

#endif // __ModuleResources_H__

