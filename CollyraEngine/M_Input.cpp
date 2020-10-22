#include "Application.h"
#include "M_Input.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "M_Resources.h"

#include "MeshLoader.h"
#include "Mesh.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"

#include "SDL/include/SDL.h"

#define MAX_KEYS 300

M_Input::M_Input(MODULE_TYPE type, bool start_enabled) : Module(type, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouseButton, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
M_Input::~M_Input()
{
	delete[] keyboard;
}

// Called before render is available
bool M_Input::Awake()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
updateStatus M_Input::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
				App->NewInputLog(i, KEY_DOWN);
			}
			else if (keyboard[i] != KEY_REPEAT)
			{
				keyboard[i] = KEY_REPEAT;
				App->NewInputLog(i, KEY_REPEAT);
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				App->NewInputLog(i, KEY_UP);
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	mouseX /= SCREEN_SIZE;
	mouseY /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouseButton[i] == KEY_IDLE)
			{
				mouseButton[i] = KEY_DOWN;
				App->NewInputLog(i, KEY_DOWN, true);
			}
			else if (mouseButton[i] != KEY_REPEAT)
			{
				mouseButton[i] = KEY_REPEAT;
				App->NewInputLog(i, KEY_REPEAT, true);
			}
		}
		else
		{
			if (mouseButton[i] == KEY_REPEAT || mouseButton[i] == KEY_DOWN)
			{
				mouseButton[i] = KEY_UP;
				App->NewInputLog(i, KEY_UP, true);
			}
			else
				mouseButton[i] = KEY_IDLE;
		}
	}

	mouseMotionX = mouseMotionY = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouseX = e.motion.x / SCREEN_SIZE;
			mouseY = e.motion.y / SCREEN_SIZE;

			mouseMotionX = e.motion.xrel / SCREEN_SIZE;
			mouseMotionY = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				break;
			}

			case (SDL_DROPFILE): 
			{     
				char* droppedFilePath = e.drop.file;

				App->resources->CreateMeshes(droppedFilePath);
				
				SDL_free(droppedFilePath);
				break;
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool M_Input::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}