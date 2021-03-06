#include "MeshLoader.h"
#include "Application.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"
#include "M_Window.h"
#include "M_Resources.h"
#include "M_UIManager.h"
#include "M_Scene.h"

#include "WG_Scene.h"

#include "Primitive.h"
#include "TextureLoader.h"
#include "C_Camera.h"

#include "OpenGL.h"
#include "MathGeoLib/include/MathGeoLib.h"

M_Renderer3D::M_Renderer3D(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled),
renderer(nullptr), frameBuffer(-1), textureBuffer(-1), depthBuffer(-1), vSync(false), playCam(nullptr)
{}

// Destructor
M_Renderer3D::~M_Renderer3D()
{}

// Called before render is available
bool M_Renderer3D::Awake()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;


	M_Window* windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);
	if (windowModule == nullptr)
	{
		ret = false;
	}

	//Create context
	context = SDL_GL_CreateContext(windowModule->window);

	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		vSync = VSYNC;

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glewInit();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		else
		{
			LOG("Using Glew %s", glewGetString(GLEW_VERSION));
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//OpenGl Additional Enables----------
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Lights Example
		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.65f, 0.65f, 0.65f, 1.0f);
		lights[0].diffuse.Set(0.15f, 0.15f, 0.15f, 1.0f);
		lights[0].Awake();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		lights[0].Active(true);
	}


	return ret;
}

// PreUpdate: clear buffer
updateStatus M_Renderer3D::PreUpdate(float dt)
{
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	M_Camera3D* cameraModule = (M_Camera3D*)App->GetModulePointer(M_CAMERA3D);
	if (cameraModule == nullptr)
	{
		return UPDATE_STOP;
	}

	float3 position;
	if (playCam != nullptr)
	{
		glLoadMatrixf(playCam->GetProjectionMatrix().ptr());
		position = playCam->GetCamPosition();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(playCam->GetViewMatrix().ptr());
	}
	else
	{
		glLoadMatrixf(cameraModule->GetProjectionMatrix().ptr());
		position = cameraModule->GetCameraPosition();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(cameraModule->GetViewMatrix().ptr());
	}


	lights[0].SetPos(position.x, position.y, position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
updateStatus M_Renderer3D::PostUpdate(float dt)
{
	updateStatus ret = UPDATE_CONTINUE;

	BeginDrawMode();

	bool* drawState = nullptr;
	drawState = App->GetDrawFlags();

	if (drawState == nullptr)
		return UPDATE_STOP;

	BeginStencilMode();
	App->PreDraw(drawState);
	EndStencilMode();

	App->Draw(drawState);
	EndDrawMode();

	//UI Render
	ret = App->Draw2D();

	M_Window* windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);
	if (windowModule == nullptr)
	{
		return UPDATE_STOP;
	}

	SDL_GL_SwapWindow(windowModule->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool M_Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	playCam = nullptr;
	renderer = nullptr;

	return true;
}

void M_Renderer3D::RefreshCamera(C_Camera* currentCam)
{
	float w, h;
	App->uiManager->sceneWindow->GetWindowSize(w, h);
	UpdateCamera(w, h, currentCam);
}

//Called when a window is alterated
void M_Renderer3D::UpdateCamera(float width, float height, C_Camera* currentCam)
{
	if (width == 0 || height == 0)
		return;

	//Calculate OpenGl projection matrix -----------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (playCam != nullptr && App->gameClock->GameRunning() == true)
	{
		playCam->SetAspectRatio(width / height);
		glLoadMatrixf(playCam->GetProjectionMatrix().ptr());
		glViewport(0, 0, width, height);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(playCam->GetViewMatrix().ptr());
	}
	else
	{
		App->camera->SetAspectRatio(width / height);
		glLoadMatrixf(App->camera->GetProjectionMatrix().ptr());
		glViewport(0, 0, width, height);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(App->camera->GetViewMatrix().ptr());
	}

	GenerateFrameBuffers(width, height);
}

void M_Renderer3D::GenerateFrameBuffers(int width, int height)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

	//Generate the depth buffer-------------
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint M_Renderer3D::GetTextureBuffer()
{
	return textureBuffer;
}

void M_Renderer3D::BeginDrawMode()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	//Grind + Axis
	CPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

}

void M_Renderer3D::EndDrawMode()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void M_Renderer3D::BeginStencilMode()
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
}

void M_Renderer3D::EndStencilMode()
{
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	glDisableClientState(GL_VERTEX_ARRAY);
}

bool M_Renderer3D::GetVSync()
{
	return vSync;
}

void M_Renderer3D::SetVSync(bool newState)
{
	vSync = newState;
}

C_Camera* M_Renderer3D::GetCurrentPlayCam() const
{
	return playCam;
}

void M_Renderer3D::SetPlayCam(C_Camera* myCam)
{
	playCam = myCam;

	if (myCam != nullptr)
	{
		RefreshCamera(myCam);
	}
	else if (App->gameClock->GameRunning() == true)
	{
		App->scene->Stop();
	}
}

void M_Renderer3D::CameraDied(C_Camera* deadCam)
{
	if (playCam != nullptr)
	{
		if (deadCam == playCam)
			if (App->gameClock->GameRunning() == true)
			{
				playCam = nullptr;
				App->scene->Stop();
			}

	}
}

float4x4 M_Renderer3D::GetCurrentViewMatrix()
{
	if (playCam != nullptr && App->gameClock->GameRunning() == true)
	{
		return playCam->GetViewMatrix();
	}
	else
	{
		return App->camera->GetViewMatrix();
	}
}

float4x4 M_Renderer3D::GetCurrentProjectionMatrix()
{
	if (playCam != nullptr && App->gameClock->GameRunning() == true)
	{
		return playCam->GetProjectionMatrix();
	}
	else
	{
		return App->camera->GetProjectionMatrix();
	}
}
