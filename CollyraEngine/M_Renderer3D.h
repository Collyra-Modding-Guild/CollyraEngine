#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class M_Camera3D;
class M_Window;
class Primitive;
class Mesh;

class M_Renderer3D : public Module
{
public:
	M_Renderer3D(MODULE_TYPE type, bool startEnabled = true);
	~M_Renderer3D();

	bool Awake();
	updateStatus PreUpdate(float dt);
	updateStatus PostUpdate(float dt);
	bool CleanUp();

	//Temp
	updateStatus Draw(float dt);
	void DrawNormals();
	updateStatus DebugDraw(float dt);

	void OnResize(float width, float height);
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed_x = 1.0f, float speed_y = 1.0f, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;

	void GenerateFrameBuffers(int width, int height);
	uint GetTextureBuffer();

	void BeginDebugMode();
	void EndDebugMode();

	void BeginDrawMode();
	void EndDrawMode();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	SDL_Renderer* renderer;

	bool drawingFaces;
	bool drawingDebugNormals;
	bool drawingTextures;

private:


	//Buffers----
	uint frameBuffer;
	uint textureBuffer;
	uint depthBuffer;

};