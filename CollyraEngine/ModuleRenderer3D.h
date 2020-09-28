#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL/include/SDL_rect.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize();
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed_x = 1.0f, float speed_y = 1.0f, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	SDL_Renderer* renderer;
};