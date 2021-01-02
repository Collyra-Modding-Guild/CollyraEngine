#ifndef __APPLICATION_CPP__
#define __APPLICATION_CPP__

#include <vector>
#include "Globals.h"
#include "p2Defs.h"

#include "PerfTimer.h"
#include "Timer.h"

#define COLLYRA_VERSION_MAJOR 3
#define COLLYRA_VERSION_MINOR 0

// Dll Paths------------ 
//Warning: The "\\" instead of the "/" or "\" is necessary, otherwise Windows doesn't recognize the paths
// TODO: This need to be in a config file *gasps* I know
#define VCVARSALL_PATH "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat"
#define COMPILE_PLATFORM "Win32"
#define COMPILE_TARGET "x86"
#define TEMP_DLL_FOLDER "C:\\temp\\CollyraEngine\\"
#define COMPILE_MODE "Debug"
#define GAMEPLAY_PROJECT_PATH "CollyraGameSystem\\CollyraGameSystem.vcxproj"
#define CLEAN_COMPILE_ACTIONS "Build"
#define REGULAR_COMPILE_ACTIONS "Rebuild"
#define GAMEPLAY_DLL_NAME "CollyraGameSystem.dll"


class Module;
class M_Window;
class M_Renderer3D;
class M_Input;
class M_Camera3D;
class M_UIManager;
class M_FileManager;
class M_Resources;
class M_Scene;
class M_Scripting;

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
	M_Scene* scene;
	M_Scripting* scriptInterface;

	float capTime = 0;

	Timer* gameClock = nullptr;

	//The amount of frames calculated in the last second
	uint64 framesOnLastUpdate = 0u;

	//The amount of time spended calculating the last frame
	uint64 lastFrameMs = 0u;

	//------ GAMEPLAY SYSTEM-------//
	HINSTANCE gameSystemDll;

private:

	int					argc;
	char** args;

	Timer				ms_timer;

	float				 gameDT;
	float				 engineDT;

	std::vector<Module*> moduleList;

	//Timers of the game
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

	float timeMultiplier;

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
	updateStatus Draw(bool* drawFlags);
	updateStatus PreDraw(bool* drawFlags);
	bool* GetDrawFlags();

	Module* GetModulePointer(MODULE_TYPE type);
	void NewInputLog(uint key, uint state, bool isMouse = false);
	void NewConsoleLog(const char* newLog);

	float GetEngineDeltaTime();
	float GetGameDeltaTime();

	float GetTimeMultiplier() const;
	void SetTimeMultiplier(float newMultiplier);


	bool CompileDll(bool cleanCompile = true);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
	void DeleteAllFilesWin(char* folderPath);

};

extern  Application* App;

#endif // __APPLICATION_CPP__