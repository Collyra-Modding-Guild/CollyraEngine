#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "MathGeoLib/include/MathGeoLibFwd.h"

#define MAX_LIGHTS 8

class M_Camera3D;
class M_Window;
class Primitive;
class Mesh;
class C_Camera;

class M_Renderer3D : public Module
{
public:
	M_Renderer3D(MODULE_TYPE type, bool startEnabled = true);
	~M_Renderer3D();

	bool Awake() override;
	updateStatus PreUpdate(float dt);
	updateStatus PostUpdate(float dt);
	bool CleanUp();

	void RefreshCamera(C_Camera* currentCam = nullptr);
	void UpdateCamera(float width, float height, C_Camera* currentCam = nullptr);

	void GenerateFrameBuffers(int width, int height);
	uint GetTextureBuffer();

	void BeginDrawMode();
	void EndDrawMode();

	void BeginStencilMode();
	void EndStencilMode();

	bool GetVSync();
	void SetVSync(bool newState);

	C_Camera* GetCurrentPlayCam() const;
	void SetPlayCam(C_Camera* myCam);

	void CameraDied(C_Camera* deadCam);

	float4x4 GetCurrentViewMatrix();
	float4x4 GetCurrentProjectionMatrix();

public:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;

	mat3x3			NormalMatrix;
	mat4x4			ModelMatrix;
	mat4x4			ViewMatrix;
	mat4x4			ProjectionMatrix;

	SDL_Renderer*	renderer;

	C_Camera*		playCam;

private:

	//Buffers----
	uint frameBuffer;
	uint textureBuffer;
	uint depthBuffer;
	bool vSync;

};