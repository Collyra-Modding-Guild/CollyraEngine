#include "Application.h"

#include "Module.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"
#include "M_FileManager.h"
#include "M_Resources.h"
#include "M_Scene.h"
#include "M_Scripting.h"

#include <fstream> 


Application::Application(int argc, char* args[]) : argc(argc), args(args), gameSystemDll(nullptr), stopExecution(false)
{
	window = new M_Window(M_WINDOW, true);
	input = new M_Input(M_INPUT, true);
	renderer3D = new M_Renderer3D(M_RENDER3D, true);
	camera = new M_Camera3D(M_CAMERA3D, true);
	uiManager = new M_UIManager(M_UIMANAGER, true);
	physFS = new M_FileManager(M_FILEMANAGER, true);
	resources = new M_Resources(M_RESOURCES, true);
	scene = new M_Scene(M_SCENE, true);
	scriptInterface = new M_Scripting(M_SCRIPTINTERFACE, true);

	gamePerfTimer = new PerfTimer();
	lastSecFrames = new Timer();
	gameClock = new Timer(false);
	timeMultiplier = 1.0f;

	// The order of calls is very important!
	// Modules will Awake() Start() and Update in this order
	// They will CleanUp() in reverse order

	if (CompileDll(true) == false)
	{
		MessageBox(0, "Dll coudn't compile on start or was not found!", "Error - Dll was not found", MB_ICONERROR);
		gameSystemDll = nullptr;
	}

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(uiManager);
	AddModule(physFS);
	AddModule(resources);
	AddModule(scriptInterface);
	AddModule(scene);

	// Scenes
	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	moduleList.clear();
}

bool Application::Awake()
{
	bool ret = true;

	if (gameSystemDll == nullptr || stopExecution == true)
		return false;

	// Call Awake() in all modules
	int numModules = moduleList.size();

	for (int i = 0; i < numModules; i++)
	{
		ret = moduleList[i]->Awake();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (int i = 0; i < numModules; i++)
	{
		if (moduleList[i]->IsEnabled())
		{
			ret = moduleList[i]->Start();
		}
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCount++;
	lastSecondFrameCount++;

	//Controls pause of the game
	if (gameClock->running)
		gameDT = lastFrameTimer.ReadSec() * timeMultiplier;
	else
		gameDT = 0.0f;

	//Controls pause of the engine
	if (!pause)
		engineDT = lastFrameTimer.ReadSec() * timeMultiplier; //Demo
	else
		engineDT = 0.0f;

	lastFrameTimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	/* float seconds_since_startup = gameTimer->ReadSec();

	avg_fps = float(frame_count) / seconds_since_startup; */

	// Amount of ms took the last update
	lastFrameMs = lastFrameTimer.Read();

	// Amount of frames during the last second
	if (lastSecFrames->Read() >= 1000)
	{
		framesOnLastUpdate = lastSecondFrameCount;
		lastSecondFrameCount = 0;
		lastSecFrames->Start();
	}

	this->uiManager->NewFpsLog(lastFrameMs, framesOnLastUpdate);

	//Waits a certain amount of time if necessary
	if (frameCap && capTime >= 1.0f)
		if (lastFrameMs < 1000 / (uint)capTime)
		{
			SDL_Delay((1000 / (uint)capTime) - lastFrameMs);
		}

}

bool Application::CompileDll(bool stopIfFailed, bool copyResult)
{
	bool ret = true;

	//Find if the vcVarsall Path if okay
	ret = FileExistsWin(VCVARSALL_PATH);

	if (ret == false)
	{
		MessageBox(0, "Failed to compile; VcVarsall path is not correct, please check the readme", "Error - Compilation Failed", MB_ICONERROR);

		if (stopIfFailed == true)
		{
			stopExecution = true;
		}

		return false;
	}

	DeleteFile("_compilation_output.txt");
	DeleteAllFilesWin(TEMP_DLL_FOLDER);
	RemoveDirectory(TEMP_DLL_FOLDER);

	char auxString[256];
	std::ofstream batch;
	batch.open("_compile.bat");

	sprintf_s(auxString, "call \"%s\" %s", VCVARSALL_PATH, COMPILE_TARGET);

	batch << auxString << std::endl;

	// TODO: We could loop this and compile for many configurations.
	// Probably not necessary thought

	sprintf_s(auxString, "MSBuild \"%s\" /t:%s /p:Configuration=%s  /p:Platform=%s /p:OutDir=%s >> _compilation_output.txt",
		GAMEPLAY_PROJECT_PATH, COMPILE_ACTIONS, COMPILE_MODE, COMPILE_PLATFORM, TEMP_DLL_FOLDER);
	batch << auxString << std::endl;

	batch.close();

	// Start process
	STARTUPINFO info = { 0 };
	PROCESS_INFORMATION procInfo = { 0 };
	ZeroMemory(&info, sizeof(info));
	info.cb = sizeof(info);
	ZeroMemory(&procInfo, sizeof(procInfo));

	DWORD retWord = 0;
	if (CreateProcessA("_compile.bat", NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &info, &procInfo))
	{
		WaitForSingleObject(procInfo.hProcess, INFINITE);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		GetExitCodeProcess(procInfo.hProcess, &retWord);
	}

	DeleteFile("_compile.bat");

	std::ifstream compilationOutput;
	std::string auxSTDSTring;
	compilationOutput.open("_compilation_output.txt");
	compilationOutput.seekg(0, std::ios::end);
	auxSTDSTring.reserve((size_t)compilationOutput.tellg());
	compilationOutput.seekg(0, std::ios::beg);
	auxSTDSTring.assign((std::istreambuf_iterator<char>(compilationOutput)), std::istreambuf_iterator<char>());
	compilationOutput.close();

	if (auxSTDSTring.find("Build FAILED") != std::string::npos || auxSTDSTring.find("ERROR al compilar") != std::string::npos)
	{
		OutputDebugStringA(auxSTDSTring.c_str());
		OutputDebugStringA("\n");
		MessageBox(0, "Failed to compile; please check your output window or _compilation_output.txt \n Keeping previous dll", "Error - Compilation Failed", MB_ICONERROR);

		ret = false;
	}
	else
	{
		if (copyResult == true)
		{
			CopyNewDll();
		}

	}

	if (gameSystemDll == NULL)
		ret = false;

	if (stopIfFailed)
		stopExecution = !ret;

	return ret;
}

bool Application::CopyNewDll()
{
	bool ret = true;

	//Free previous library (if any)
	bool isFree = FreeLibrary(gameSystemDll);
	gameSystemDll = 0;

	LOG("Compilation Completed! Copying file to engine folder...");
	std::string tmpDllLocation = std::string(TEMP_DLL_FOLDER).append(GAMEPLAY_DLL_NAME);

	bool result = CopyFileA(tmpDllLocation.c_str(), GAMEPLAY_DLL_NAME, false);
	DWORD error = GetLastError();
	HRESULT errorTranslate = HRESULT_FROM_WIN32(error);

	if (result == false)
	{
		LOG("Error copying file, check the source path!");
		ret = false;
	}

	gameSystemDll = LoadLibrary(std::string(GAMEPLAY_DLL_NAME).data());

	LOG("Deleting temp folders & files...");
	DeleteAllFilesWin(TEMP_DLL_FOLDER);
	RemoveDirectory(TEMP_DLL_FOLDER);
	DeleteFile("_compilation_output.txt");

	return ret;

}

// Call PreUpdate, Update and PostUpdate on all modules
updateStatus Application::Update()
{
	updateStatus ret = UPDATE_CONTINUE;
	PrepareUpdate();

	//PreUpdate
	int numModules = moduleList.size();

	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->PreUpdate(engineDT);
	}

	//Update
	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->Update(engineDT);
	}

	//PostUpdate
	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->PostUpdate(engineDT);
	}

	if (stopExecution == true)
	{
		return UPDATE_STOP;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	int numModules = moduleList.size();

	for (int i = numModules - 1; i >= 0 && ret == true; i--)
	{
		ret = moduleList[i]->CleanUp();

	}

	moduleList.clear();

	return ret;
}

bool Application::Reset()
{
	bool ret = true;
	int numModules = moduleList.size();


	for (int i = 0; i < numModules && ret == true; i++)
	{
		ret = moduleList[i]->Reset();
	}

	return ret;
}

updateStatus Application::Draw2D()
{
	updateStatus ret = UPDATE_CONTINUE;

	int numModules = moduleList.size();

	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->Draw2D(engineDT);
	}

	return ret;
}

updateStatus Application::Draw(bool* drawFlags)
{
	updateStatus ret = UPDATE_CONTINUE;

	int numModules = moduleList.size();

	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->Draw(drawFlags);
	}

	return ret;
}

updateStatus Application::PreDraw(bool* drawFlags)
{
	updateStatus ret = UPDATE_CONTINUE;

	int numModules = moduleList.size();

	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->PreDraw(drawFlags);
	}

	return ret;
}

bool* Application::GetDrawFlags()
{
	if (uiManager != nullptr)
	{
		return(uiManager->GetDrawFlags());
	}

	return nullptr;
}

Module* Application::GetModulePointer(MODULE_TYPE type)
{
	Module* ret = nullptr;

	switch (type)
	{
	case NO_TYPE:
	{
		return nullptr;
	}
	break;
	case M_WINDOW:
	{
		if (window != nullptr)
			ret = window;
	}
	break;
	case M_RENDER3D:
	{
		if (renderer3D != nullptr)
			ret = renderer3D;
	}
	break;
	case M_INPUT:
	{
		if (input != nullptr)
			ret = input;
	}
	break;
	case M_CAMERA3D:
	{
		if (camera != nullptr)
			ret = camera;
	}
	break;
	case M_UIMANAGER:
	{
		if (uiManager != nullptr)
			ret = uiManager;
	}
	break;
	case M_SCRIPTINTERFACE:
	{
		if (scriptInterface != nullptr)
			ret = scriptInterface;
	}
	break;
	default:
		return nullptr;
		break;
	}

	if (ret == nullptr || ret->GetType() != type)
		return nullptr;

	return ret;
}

void Application::NewInputLog(uint key, uint state, bool isMouse)
{
	if (uiManager != nullptr)
	{
		uiManager->NewInputLog(key, state, isMouse);
	}
}

void Application::NewConsoleLog(const char* newLog)
{
	if (uiManager != nullptr)
	{
		uiManager->NewConsoleLog(newLog);
	}
}

float Application::GetEngineDeltaTime()
{
	return engineDT;
}

float Application::GetGameDeltaTime()
{
	return gameDT;
}

float Application::GetTimeMultiplier() const
{
	return timeMultiplier;
}

void Application::SetTimeMultiplier(float newMultiplier)
{
	timeMultiplier = newMultiplier;
}

void Application::AddModule(Module* mod)
{
	moduleList.push_back(mod);
}

void Application::DeleteAllFilesWin(char* folderPath)
{
	char fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hp;
	sprintf_s(fileFound, "%s\\*.*", folderPath);
	hp = FindFirstFile(fileFound, &info);
	if (hp != INVALID_HANDLE_VALUE)
	{
		do
		{
			sprintf_s(fileFound, "%s\\%s", folderPath, info.cFileName);
			DeleteFile(fileFound);

		} while (FindNextFile(hp, &info));
		FindClose(hp);
	}
}

bool Application::FileExistsWin(const char* file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		FindClose(handle);
	}
	return found;
}
