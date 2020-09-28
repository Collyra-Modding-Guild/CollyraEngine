#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	tex = new ModuleTextures(this);
	fonts = new ModuleFonts(this);
	scene_intro = new ModuleSceneIntro(this, true, 5u);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this, true, 1);
	camera_2 = new ModuleCamera3D(this, true, 2);
	physics = new ModulePhysics3D(this);
	player = new ModulePlayer(this, true, 1);
	player_2 = new ModulePlayer(this, true, 2);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(fonts);
	AddModule(tex);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(player);
	AddModule(player_2);
	AddModule(camera);
	AddModule(camera_2);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	p2List_item<Module*>* item = list_modules.getFirst();

	while(item != NULL && ret == true)
	{
		ret = item->data->Init();
		item = item->next;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.getFirst();

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	p2List_item<Module*>* item = list_modules.getFirst();
	
	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PreUpdate(dt);
		item = item->next;
	}

	item = list_modules.getFirst();

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}

	item = list_modules.getFirst();

	while(item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PostUpdate(dt);
		item = item->next;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	return ret;
}

bool Application::Draw()
{
	bool ret = true;

	p2List_item<Module*>* item = list_modules.getFirst();

	while (item != NULL && ret == true)
	{
		ret = item->data->Draw();
		item = item->next;
	}
	return ret;
}

bool Application::Reset()
{
	bool ret = true;

	p2List_item<Module*>* item = list_modules.getFirst();

	while (item != NULL && ret == true)
	{
		ret = item->data->Reset();
		item = item->next;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.add(mod);
}
