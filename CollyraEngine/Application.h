#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include <vector>
#include "Globals.h"
#include "p2Defs.h"

#include "PerfTimer.h"
#include "Timer.h"

class Module;
class M_Window;
class M_Renderer3D;
class M_Input;
class M_Camera3D;
class M_UIManager;
class M_FileManager;
class M_Resources;

enum MODULE_TYPE;

class Application
{
public:
	M_Window* window;
	M_Renderer3D* renderer3D;
	M_Input* input;
	M_Camera3D* camera;
	M_UIManager* uiManager;
	M_FileManager* physFS;
	M_Resources* resources;
	
	float capTime = 0;

	//The amount of frames calculated in the last second
	uint64 framesOnLastUpdate = 0u;

	//The amount of time spended calculating the last frame
	uint64 lastFrameMs = 0u;

private:

	int					argc;
	char**				args;

	Timer				ms_timer;
	float				dt;
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

	Application(int argc, char* args[]);
	~Application();

	bool Awake();
	updateStatus Update();
	bool CleanUp();
	bool Reset();

	updateStatus Draw2D();
	updateStatus Draw();
	updateStatus DebugDraw();
	bool IsDebugModeOn();

	Module* GetModulePointer(MODULE_TYPE type);
	void NewInputLog(uint key, uint state, bool isMouse = false);
	void NewConsoleLog(const char* newLog);


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

};

extern Application* App;

#endif // __APPLICATION_CPP__