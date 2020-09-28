
#ifndef __MODULE_FONTS_H__
#define __MODULE_FONTS_H__

#include "Module.h"
#include "p2List.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class ModuleFonts : public Module
{
public:

	ModuleFonts(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleFonts();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	p2List<_TTF_Font*>	fonts;
	_TTF_Font* default;
};


#endif // __MODULE_FONTS_H__
