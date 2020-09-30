#pragma once

#include <vector>
#include "Globals.h"
#include "p2Defs.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUIManager.h"

#include "ModuleTextures.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleTextures* tex;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleUIManager* uiManager;

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
	bool Draw();
	bool Reset();




private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};