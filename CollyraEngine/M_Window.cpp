#include "Application.h"
#include "M_UIManager.h"
#include "M_Window.h"
#include "Globals.h"

#include "SDL/include/SDL_video.h"


M_Window::M_Window(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), brightness(0.0f), screenWidth(1280), screenHeight(1024),
fullscreen(false), resizable(true), borderless(false), fullscreenDesktop(false)
{
	window = NULL;	
	screen_surface = NULL;

	winTitle = nullptr;
	orgTitle = "Collyra Guild";
}

// Destructor
M_Window::~M_Window()
{
}

// Called before render is available
bool M_Window::Awake()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = screenWidth * SCREEN_SIZE;
		int height = screenHeight * SCREEN_SIZE;

		winTitle = TITLE;

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL Attributes + Version

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(winTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		OnResize();
	}

	return ret;
}

// Called before quitting
bool M_Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void M_Window::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}


void M_Window::SetFullscreen(bool enabled) 
{
	if (enabled) 
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else 
		SDL_SetWindowFullscreen(window, 0);
}

void M_Window::SetFullscreenDesktop(bool enabled)
{
	if (enabled)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}



void M_Window::SetBorderless(bool enabled)
{
	SDL_SetWindowBordered(window, (SDL_bool)!enabled);
}

void M_Window::OnResize()
{
	SDL_GetWindowSize(window, &screenWidth, &screenHeight);

}
