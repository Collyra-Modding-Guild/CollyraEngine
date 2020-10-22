#include "MeshLoader.h"
#include "Application.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"
#include "M_Window.h"

#include "Primitive.h"
#include "Mesh.h"
#include "TextureLoader.h"

#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

M_Renderer3D::M_Renderer3D(MODULE_TYPE type, bool start_enabled) : Module(type, start_enabled),
renderer(nullptr), frameBuffer(-1), textureBuffer(-1), depthBuffer(-1), drawingFaces(true), drawingDebugNormals(false)
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

	AddMeshes(MeshLoader::Load("warrior/warrior.fbx"));

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

	//Generate the depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

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

	// light 0 on cam pos

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

	if (drawingDebugNormals == true)
	{
		DrawNormals();
	}

	//Debug Render
	if (App->IsDebugModeOn() == true)
	{
		BeginDebugMode();
		App->DebugDraw();
		EndDebugMode();
	}

	if (drawingFaces == true)
	{
		App->Draw();
	}

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

	int numPrimitives = primitives.size();

	for (int i = numPrimitives - 1; i >= 0; i--)
	{
		RELEASE(primitives[i]);
	}

	primitives.clear();
	meshes.clear();

	return true;
}

updateStatus M_Renderer3D::Draw(float dt)
{
	updateStatus ret = updateStatus::UPDATE_CONTINUE;

	for (uint i = 0; i < primitives.size(); i++)
	{
		primitives[i]->Render();
	}

	for (uint i = 0; i < meshes.size(); i++)
	{
		meshes[i].Render();
	}

	return ret;
}

void M_Renderer3D::DrawNormals()
{
	for (uint i = 0; i < meshes.size(); i++)
	{
		meshes[i].DrawNormals();
	}
}

updateStatus M_Renderer3D::DebugDraw(float dt)
{
	updateStatus ret = updateStatus::UPDATE_CONTINUE;

	for (uint i = 0; i < primitives.size(); i++)
	{
		primitives[i]->Render(true);
	}

	for (uint i = 0; i < meshes.size(); i++)
	{
		meshes[i].Render(true);
	}
	return ret;
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

// Blit to screen
bool M_Renderer3D::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed_x, float speed_y, SDL_RendererFlip flip, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	SDL_Rect rect;
	rect.x = (int)x;
	rect.y = (int)y;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= 10;
	rect.h *= 10;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}


	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

void M_Renderer3D::BeginDebugMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void M_Renderer3D::EndDebugMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void M_Renderer3D::AddMeshes(std::vector<Mesh>& newMeshes)
{
	if (newMeshes.size() > 0)
	{
		int meshesEnd = meshes.size();
		meshes.insert(meshes.end(), newMeshes.begin(), newMeshes.end());

		for (meshesEnd; meshesEnd < meshes.size(); meshesEnd++)
		{
			meshes[meshesEnd].SetTextureId(TextureLoader::LoadDefaultTexture());
		}
	}

}
