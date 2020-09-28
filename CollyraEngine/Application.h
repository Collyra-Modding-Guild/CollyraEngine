#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModuleFonts.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleTextures* tex;
	ModuleFonts* fonts;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleCamera3D* camera_2;
	ModulePhysics3D* physics;
	ModulePlayer* player;
	ModulePlayer* player_2;

private:

	Timer	ms_timer;
	float	dt;
	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	bool Draw();
	bool Reset();


	p2List<ModuleCamera3D*> cam_list;


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};