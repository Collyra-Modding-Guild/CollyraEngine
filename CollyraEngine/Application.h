#pragma once

#include <vector>
#include "Globals.h"
#include "p2Defs.h"
#include "PerfTimer.h"
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

	float capTime = 0;

	//The amount of frames calculated in the last second
	uint64 frames_on_last_update = 0u;

	//The amount of time spended calculating the last frame
	uint64 last_frame_ms = 0u;


private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> moduleList;

	//Timers of the game
	Timer* engineTimer = nullptr;
	PerfTimer* gamePerfTimer = nullptr;

	//Calculates the amount of frames rendered in the last second
	Timer* lastSecFrames = nullptr;

	//The amount of time spended in the calculation of the last frame
	Timer lastFrameTimer;

	//The amount of frames rendered since the game started
	uint64 frameCount = 0u;

	//The amount of frames rendered last second
	uint lastSecondFrameCount = 0u;



	float avgFps = 0.0f;

	//Controls if the frame cap is activated or not
	bool frameCap = true;

	//-----FRAME CONTROL------//
	bool windowTitleControl = false; //Changes the title of the window
	bool pause = false; //Bool that controls the pause of the game

public:

	Application();
	~Application();

	bool Awake();
	updateStatus Update();
	bool CleanUp();
	bool Reset();

	
	float GetDeltaTime() const;

	//Used to change the frame cap
	void UpdateFrameCap(uint newCap);



private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};