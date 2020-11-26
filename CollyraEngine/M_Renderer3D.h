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

	void RefreshCamera();
	void UpdateCamera(float width, float height);

	void GenerateFrameBuffers(int width, int height);
	uint GetTextureBuffer();

	void BeginDrawMode();
	void EndDrawMode();

	void BeginStencilMode();
	void EndStencilMode();

	bool GetVSync();
	void SetVSync(bool newState);

public:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;

	mat3x3			NormalMatrix;
	mat4x4			ModelMatrix;
	mat4x4			ViewMatrix;
	mat4x4			ProjectionMatrix;

	SDL_Renderer*	renderer;

private:

	//Buffers----
	uint frameBuffer;
	uint textureBuffer;
	uint depthBuffer;
	bool vSync;

};