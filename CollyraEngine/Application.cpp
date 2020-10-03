#include "Application.h"


Application::Application()
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
	AddModule(uiManager);
	AddModule(input);

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
	frame_count++;
	last_second_frame_count++;

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
	last_frame_ms = lastFrameTimer.Read();

	// Amount of frames during the last second
	if (lastSecFrames->Read() >= 1000)
	{
		frames_on_last_update = last_second_frame_count;
		last_second_frame_count = 0;
		lastSecFrames->Start();
	}

	//Waits a certain amount of time if necessary
	if (frameCap && capTime > 0)
		if (last_frame_ms < 1000 / (uint)capTime)
		{
			SDL_Delay((1000 / (uint)capTime) - last_frame_ms);
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

void Application::AddModule(Module* mod)
{
	moduleList.push_back(mod);
}
