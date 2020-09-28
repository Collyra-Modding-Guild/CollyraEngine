#ifndef __MODULE_TEXTURES_H__
#define __MODULE_TEXTURES_H__

#include "Module.h"
#include <vector>

struct SDL_Texture;
struct SDL_Surface;

class ModuleTextures : public Module
{
public:

	ModuleTextures(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleTextures();

	// Called before render is available
	bool Init();

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const	Load(const char* path);
	bool				UnLoad(SDL_Texture* texture);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

public:

	std::vector<SDL_Texture*>	textures;
};


#endif // __MODULE_TEXTURES_H__
