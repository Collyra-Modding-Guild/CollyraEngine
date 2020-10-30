#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>

class WindowGroup;
class WG_Config;
class WG_Console;
class WG_Scene;
class WG_Hierarchy;
class WG_Inspector;
class WG_About;


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
	void            SetFocusedGameObject(uint id);
	int				GetFocusedGameObjectId() const;

private:
	//Functions for the editor menus-----------


	bool			ShowMainMenuBar();
	void			EnableDockSpace();

public:

	std::vector<WindowGroup*> windowGroups;

	WG_Config*		configWindow;
	WG_Console*		consoleWindow;
	WG_Scene*		sceneWindow;
	WG_Hierarchy*	hierarchyWindow;
	WG_Inspector*	inspectorWindow;
	WG_About*		aboutWindow;

private:
	//Bool controllers for the editor menus-----------
	bool			showDemoWindow;
	bool			showConfigMenu;

};

#endif // __ModuleAudio_H__
