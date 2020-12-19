#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"
#include <string>
#include <vector>

#include "MathGeoLib/include/MathGeoLibFwd.h"


class WindowGroup;
class WG_Config;
class WG_Console;
class WG_Scene;
class WG_Hierarchy;
class WG_Inspector;
class WG_About;
class WG_Playbar;
class WG_ResourceCount;
class WG_Assets;

struct PathNode;

class M_UIManager : public Module
{
public:

	M_UIManager(MODULE_TYPE type, bool startEnabled = true);
	~M_UIManager();

	bool Awake();
	bool Start();

	updateStatus PreUpdate(float dt);
	updateStatus Update(float dt);

	bool CleanUp();

	updateStatus Draw2D(float dt);

	// Window Comunication--------
	void			NewInputLog(uint keyID, uint keyState, bool isMouse = false);
	void			NewConsoleLog(const char* newLog);
	void			NewFpsLog(float currMs, float currFps);
	bool*			GetDrawFlags();
	void			GetWindowSceneSize(float& w, float& h);
	void			GameObjectDestroyed(uint id);
	void            SetFocusedGameObject(int id);
	int				GetFocusedGameObjectId() const;
	void			OnWindowResize() const;

	float2			GetSceneWindowSize() const;
	float2			GetSceneWindowPosition() const;
	float2			GetImGuiMousePosition() const;

	void SetupSmoothImGuiStyle(bool volumeEffect);

	void SetupDarkImGuiStyle(float alpha);
	void SetupLightImGuiStyle();
	std::string DrawDirectoryRecursiveOld(const char* directory, bool returnFullPath = false, std::vector<std::string>* ignoreExt = nullptr, char* dragType = nullptr);
	std::string DrawDirectoryRecursive(PathNode* myNode, const char* directory, bool returnFullPath = false, std::vector<std::string>* ignoreExt = nullptr, char* dragType = nullptr);

private:

	//Functions for the editor menus-----------
	bool			ShowMainMenuBar();
	void			EnableDockSpace();

	//Pop Ups--------------------------
	void			ShowLoadScenePopUp();
	void			ShowSelectedItemPopUp();

	// Helpers ----
	std::string		GetTreeFormatedName(std::string fullPath);

public:

	std::vector<WindowGroup*> windowGroups;

	WG_Config*		configWindow;
	WG_Console*		consoleWindow;
	WG_Scene*		sceneWindow;
	WG_Hierarchy*	hierarchyWindow;
	WG_Inspector*	inspectorWindow;
	WG_About*		aboutWindow;
	WG_Playbar*		playWindow;
	WG_ResourceCount* resourceCount;
	WG_Assets*		assetsWindow;


private:
	//Bool controllers for the editor menus-----------
	bool			showDemoWindow;
	bool			showConfigMenu;
	unsigned int	lastSavedId;
	bool			showLoadScenePop;
	std::string		selectedMenuNode;
};

#endif // __ModuleAudio_H__
