#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class M_Window : public Module
{
public:

	M_Window(MODULE_TYPE type, bool start_enabled = true);

	// Destructor
	virtual ~M_Window();

	bool Awake();
	bool CleanUp();

	void SetTitle(const char* title);

	void SetFullscreen(bool enabled);
	void SetFullscreenDesktop(bool enabled);
	void SetBorderless(bool enabled);


public:
	//The window we'll be rendering to
	SDL_Window* window;

	int screenWidth;
	int screenHeight;

	float brightness;

	char* winTitle;
	char* orgTitle;

	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fullscreenDesktop;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__