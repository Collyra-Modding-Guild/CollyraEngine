#include "Application.h"


Application::Application(int argc, char* args[]) : argc(argc), args(args)
{
	window = new M_Window(this);
	input = new M_Input(this);
	renderer3D = new M_Renderer3D(this);
	camera = new M_Camera3D(this, true);
	uiManager = new M_UIManager(this, true);
	engineTimer = new Timer();
	gamePerfTimer = new PerfTimer();
	lastSecFrames = new Timer();

	// The order of calls is very important!
	// Modules will Awake() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(uiManager);

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
	if (!pause)
		dt = lastFrameTimer.ReadSec();
	else
		dt = 0.0f;

	lastFrameTimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	/*float seconds_since_startup = gameTimer->ReadSec();

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

// Call PreUpdate, Update and PostUpdate on all modules
updateStatus Application::Update()
{
	updateStatus ret = UPDATE_CONTINUE;
	PrepareUpdate();

	//PreUpdate
	int numModules = moduleList.size();

	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->PreUpdate(dt);
	}

	//Update
	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->Update(dt);
	}

	//PostUpdate
	for (int i = 0; i < numModules && ret == UPDATE_CONTINUE; i++)
	{
		ret = moduleList[i]->PostUpdate(dt);
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
		ret = moduleList[i]->Draw2D(dt);
	}

	return ret;
}

bool Application::IsDebugModeOn()
{
	if (App->uiManager != nullptr)
	{
		return(App->uiManager->IsDebugModeOn());
	}

	return false;
}

void Application::AddModule(Module* mod)
{
	moduleList.push_back(mod);
}
