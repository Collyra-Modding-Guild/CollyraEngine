#include "MeshLoader.h"
#include "Application.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"
#include "M_Window.h"
#include "M_Resources.h"

#include "Primitive.h"
#include "TextureLoader.h"

#include "OpenGL.h"

M_Renderer3D::M_Renderer3D(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled),
renderer(nullptr), frameBuffer(-1), textureBuffer(-1), depthBuffer(-1)
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
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].Awake();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		lights[0].Active(true);
	}


	return ret;
}

void M_Renderer3D::GenerateFrameBuffers(int width, int height)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

// PreUpdate: clear buffer
updateStatus M_Renderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	M_Camera3D* cameraModule = (M_Camera3D*)App->GetModulePointer(M_CAMERA3D);
	if (cameraModule == nullptr)
	{
		return UPDATE_STOP;
	}

	glLoadMatrixf(cameraModule->GetViewMatrix());

	lights[0].SetPos(cameraModule->Position.x, cameraModule->Position.y, cameraModule->Position.z);

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

	return true;
}

//Called when a window is alterated
void M_Renderer3D::OnResize(float width, float height)
{
	if (width == 0 || height == 0)
		return;

	//Calculate OpenGl projection matrix -----------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, width / height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	GenerateFrameBuffers(width, height);
}

void M_Renderer3D::BeginDrawMode()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Grind + Axis
	CPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
}

void M_Renderer3D::EndDrawMode()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
