#pragma once

#include <vector>
#include "Globals.h"
#include "p2Defs.h"
#include "Timer.h"
#include "Module.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"




class Application
{
public:
	M_Window* window;
	M_Renderer3D* renderer3D;
	M_Input* input;
	M_Camera3D* camera;
	M_UIManager* uiManager;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	bool Reset();




private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};