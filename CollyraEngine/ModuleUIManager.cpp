#include "Globals.h"
#include "Application.h"
#include "ModuleUIManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"

#include "imgui/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

//#pragma comment (lib, "imgui/libs/glew/glew32.lib")

ModuleUIManager::ModuleUIManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleUIManager::~ModuleUIManager()
{}

// Called before render is available
bool ModuleUIManager::Init()
{

	
	return true;
}

update_status ModuleUIManager::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleUIManager::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleUIManager::CleanUp()
{

	return true;
}


	