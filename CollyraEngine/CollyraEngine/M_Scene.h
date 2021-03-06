#ifndef __ModuleScene_H__
#define __ModuleScene_H__

#include "Module.h"
#include <vector>
#include <string>
#include "p2Defs.h"
#include <map>

#include "MathGeoLib/include/Algorithm/Random/LCG.h"
#include "MathGeoLib/include/Geometry/LineSegment.h"

#define ROOT_ID 0
#define DEFAULT_SCENE_NAME "DefaultScene"

class GameObject;
class Component;
class C_Camera;
class C_Script;
class R_Scene;
enum class R_TYPE;

class M_Scene : public Module
{
public:

	M_Scene(MODULE_TYPE type, bool startEnabled = true);
	~M_Scene();

	bool Awake() override;
	bool Start() override;
	updateStatus Update(float dt) override;
	updateStatus PostUpdate(float dt) override;
	updateStatus Draw(bool* drawState) override;
	updateStatus PreDraw(bool* drawState) override;

	bool CleanUp() override;

	GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr);
	GameObject* GetGameObject(unsigned int id);
	GameObject* GetGameObject(std::string name);
	std::vector<GameObject*> GetAllGameobjects();
	bool		DeleteGameObject(unsigned int id);
	bool		DeleteGameObject(GameObject* gameObject);
	bool		NotifyGameObjectDeath(GameObject* gameObject);
	void		DeleteCamera(Component* component);
	template<typename T>
	std::vector<T*> GetAllComponents();
	void		StartGameObjects();

	void		OnClickFocusGameObject(const LineSegment& mouseRay);

	bool		CameraCuling(GameObject* current, C_Camera* myCam);

	// Game State ------
	void Play();
	updateStatus StartPlayMode() override;
	void Stop();
	updateStatus StopPlayMode() override;
	void Pause();
	void Resume();
	void Tick();

	//Delete all GameObjects, Components, cameras, reset Name,....
	void		ResetScene();
	uint		SaveScene();

	uint32 GenerateId();

	GameObject* GetRoot();	

	std::string GetSceneName() const;
	void SetSceneName(const char* newName);
	R_Scene* GetSceneResource() const;
	void SetSceneResource(R_Scene* newScene);

	void ResoucesUpdated(std::map<uint, bool>* updatedId);
	void SetResourceToGameObject(uint resourceId, R_TYPE rType, GameObject* setTo = nullptr);

	void GenerateNewScene();

	//Gameplay System
	void PreHotReload();
	void PostrHotReload();

private:
	void CheckSiblingsName(GameObject* parent, std::string& myName);
	void DrawGameObject(GameObject* gameObject, bool* drawState);
	void DrawBoundingBox(GameObject* gameObject);
	void DrawFrustum(GameObject* gameObject);
	void DrawMouseRay(LineSegment* mouseRay);

public:
	GameObject* focusedGameObject;
	std::vector<C_Camera*> cameras;
	LCG randomGenerator;

private:
	R_Scene* currentScene;
	uint playedScene;
	std::string savedScenePath;

	std::vector<C_Script*> allScriptComponents;
};

#endif // __ModuleResources_H__

