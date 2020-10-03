#include "Globals.h"
#include "Application.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "M_Window.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

#pragma comment (lib, "Glew/libx86/glew32.lib")



M_UIManager::M_UIManager(Application* app, bool start_enabled) : Module(app, start_enabled), showDemoWindow(false), menuMathRandomTest(false), generateRandomNumbers(false), generatedInt(0), generatedFloat(0.0f), 
fps_log(100), ms_log(100), sphereCollisionTest(false)
{}

// Destructor
M_UIManager::~M_UIManager()
{}

// Called before render is available
bool M_UIManager::Awake()
{

	//Demo-------------------
	showDemoWindow = false;

	randomSeed = LCG::LCG();
	randomStartThreshold = 0;
	randomEndThreshold = 1000;

	sph1 = Sphere({ 0.f,0.f,0.f }, 10);
	sph2 = Sphere({ 0.f,5.f,0.f }, 10);

	boundingBox1 = AABB({ 0.f, 0.f, 0.f }, { 10.f, 10.f, 10.f });
	boundingBox1 = AABB({ 0.f, 0.f, 0.f }, { 5.f, 5.f, 5.f });


	return true;
}

bool M_UIManager::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

updateStatus M_UIManager::PreUpdate(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();


	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;

	static char string[26];
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Configuration");

	if (ImGui::CollapsingHeader("Application"))
	{

		// - - - - - - - App Title - - - - - - - - -
		strcpy_s(string, 26, App->window->winTitle);
		if (ImGui::InputText("App Name", string, 26))
			App->window->SetTitle(string);

		// - - - - - - - Organization - - - - - - - - -
		strcpy_s(string, 26, App->window->orgTitle);
		ImGui::InputText("Organization", string, 26);

		ImGui::Separator(); // - - - - - - - - - - - - -

		// - - - - - - - - FPS Cap - - - - - - - - - -
		ImGui::SliderFloat("Limit Framerate", &App->capTime, 0, 125);
		ImGui::Text("Limit FPS: %i", int(App->capTime));

		// - - - - - - - - Framerate Stuff Graphs - - - - - - - - - -
		sprintf_s(string, 26, "Framerate %.1f: ", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], 100, 0, string, 0.0f, 100.0f, ImVec2(310, 100));

		sprintf_s(string, 26, "Miliseconds %0.1f: ", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##miliseconds", &ms_log[0], 100, 0, string, 0.0f, 40.0f, ImVec2(310, 100));

	}	

	if (ImGui::CollapsingHeader("Window"))
	{
		// - - - - - - - - Window Brightness - - - - - - - - - -
		if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.0f, 1.0f))
			SDL_SetWindowBrightness(App->window->window, App->window->brightness);
		
		// - - - - - - - - Screen Surface - - - - - - - - - -
		if (ImGui::SliderInt("Width", &App->window->SCREEN_WIDTH, 0, 1920))
			SDL_SetWindowSize(App->window->window, App->window->SCREEN_WIDTH, App->window->SCREEN_HEIGHT);

		if (ImGui::SliderInt("Height", &App->window->SCREEN_HEIGHT, 0, 1080))
			SDL_SetWindowSize(App->window->window, App->window->SCREEN_WIDTH, App->window->SCREEN_HEIGHT);
		
		// - - - - - - - - Display Modes - - - - - - - - - -
		if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
			App->window->SetFullscreen(App->window->fullscreen);

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &App->window->resizable))
			App->window->SetResizable(App->window->resizable);

		if (ImGui::Checkbox("Borderless", &App->window->borderless))
			App->window->SetBorderless(App->window->borderless);

		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &App->window->fullscreen_desktop))
			App->window->SetFullscreenDesktop(App->window->fullscreen_desktop);


	
	}

	if (ImGui::CollapsingHeader("File System"))
	{

	}

	if (ImGui::CollapsingHeader("Input"))
	{

	}

	if (ImGui::CollapsingHeader("Hardware"))
	{

	}
	

	ImGui::End();
	

	return stopAppButton();
}

updateStatus M_UIManager::Update(float dt)
{

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	//-------------------------

	ShowMainMenuBar();

	if (menuMathRandomTest)
		ShowMenuRandomTest();

	if (sphereCollisionTest)
		ShowMenuSphereCollisionTest();

	if (AABBCollisionTest)
		ShowMenuAABBCollisions();

	return UPDATE_CONTINUE;
}

updateStatus M_UIManager::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}


	return UPDATE_CONTINUE;
}

// Called before quitting
bool M_UIManager::CleanUp()
{

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();


	return true;
}

bool M_UIManager::ShowMenuRandomTest()
{
	if (!ImGui::Begin("Random Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}

	ImGui::Text("Generate Numbers between:");
	ImGui::SameLine();
	ImGui::PushItemWidth(120);
	ImGui::InputInt(" ", &randomStartThreshold);
	ImGui::SameLine();
	ImGui::Text("And");
	ImGui::SameLine();
	ImGui::InputInt("  ", &randomEndThreshold);


	if (ImGui::Button("Generate Numbers"))
	{
		generateRandomNumbers = !generateRandomNumbers;
	}

	if (generateRandomNumbers)
	{
		generatedInt = randomSeed.Int(randomStartThreshold, randomEndThreshold);
		generatedFloat = randomSeed.Float(randomStartThreshold, randomEndThreshold);
	}

	ImGui::SameLine();
	if (ImGui::Button("Generate 1 Number"))
	{
		generatedInt = randomSeed.Int(randomStartThreshold, randomEndThreshold);
		generatedFloat = randomSeed.Float(randomStartThreshold, randomEndThreshold);

		generateRandomNumbers = false;
	}


	ImGui::Text("Random Int : %i", generatedInt);
	ImGui::SameLine();
	ImGui::Text("Random Float : %f", generatedFloat);

	ImGui::End();
}

bool M_UIManager::ShowMenuSphereCollisionTest()
{
	if (!ImGui::Begin("Sphere Collision Menu", &sphereCollisionTest))
	{
		ImGui::End();
		return false;
	}


	if (ImGui::Button("Generate New Spheres"))
	{
		sph1.pos = { randomSeed.Float(0,10), randomSeed.Float(0,10), randomSeed.Float(0,10) };
		sph2.pos = { randomSeed.Float(0,10), randomSeed.Float(0,10), randomSeed.Float(0,10) };
		sph1.r = randomSeed.Float(1, 5);
		sph2.r = randomSeed.Float(1, 5);
	}

	ImGui::Text("Sphere 1 \n Position: %.2f, %f, %.2f Radius: %.2f", sph1.pos.x, sph1.pos.y, sph1.pos.z, sph1.r);

	ImGui::Text("Sphere 2 \n Position: %f, %.2f, %f Radius: %.2f", sph2.pos.x, sph2.pos.y, sph2.pos.z, sph2.r);

	ImGui::NewLine();

	if (sph1.Intersects(sph2))
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}

bool M_UIManager::ShowMenuAABBCollisions()
{
	if (!ImGui::Begin("Random Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}


	if (ImGui::Button("Generate New AABB"))
	{
		boundingBox1.minPoint.x = randomSeed.Float(0, 5);
		boundingBox1.minPoint.y = randomSeed.Float(0, 5);
		boundingBox1.minPoint.z = randomSeed.Float(0, 5);

		boundingBox1.maxPoint.x = randomSeed.Float(5, 10);
		boundingBox1.maxPoint.y = randomSeed.Float(5, 10);
		boundingBox1.maxPoint.z = randomSeed.Float(5, 10);

		boundingBox2.minPoint.x = randomSeed.Float(5, 10);
		boundingBox2.minPoint.y = randomSeed.Float(5, 10);
		boundingBox2.minPoint.z = randomSeed.Float(5, 10);

		boundingBox2.maxPoint.x = randomSeed.Float(10, 15);
		boundingBox2.maxPoint.y = randomSeed.Float(10, 15);
		boundingBox2.maxPoint.z = randomSeed.Float(10, 15);

	}

	ImGui::Text("AABB 1 \n MinSize: %.2f, %f, %.2f \n MaxSize: %.2f, %f, %.2f", boundingBox1.minPoint.x, boundingBox1.minPoint.y, boundingBox1.minPoint.z,
		boundingBox1.maxPoint.x, boundingBox1.maxPoint.y, boundingBox1.maxPoint.z);

	ImGui::Text("AABB 2 \n MinSize: %.2f, %f, %.2f \n MaxSize: %.2f, %f, %.2f", boundingBox2.minPoint.x, boundingBox2.minPoint.y, boundingBox2.minPoint.z,
		boundingBox2.maxPoint.x, boundingBox2.maxPoint.y, boundingBox2.maxPoint.z);

	ImGui::NewLine();

	if (boundingBox1.Intersection(boundingBox2).Volume() > 0)
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}


void M_UIManager::ShowMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			//ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Examples"))
		{
			ImGui::MenuItem("Main menu bar", NULL, true);
			ImGui::MenuItem("Console", NULL, true);
			ImGui::MenuItem("Log", NULL, true);
			ImGui::MenuItem("Simple layout", NULL, true);
			ImGui::MenuItem("Property editor", NULL, true);
			ImGui::MenuItem("Long text display", NULL, true);
			ImGui::MenuItem("Auto-resizing window", NULL, true);
			ImGui::MenuItem("Constrained-resizing window", NULL, true);
			ImGui::MenuItem("Simple overlay", NULL, true);
			ImGui::MenuItem("Manipulating window titles", NULL, true);
			ImGui::MenuItem("Custom rendering", NULL, true);
			ImGui::MenuItem("Dockspace", NULL, true);
			ImGui::MenuItem("Documents", NULL, true);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", NULL, true);
			ImGui::MenuItem("Style Editor", NULL, true);
			ImGui::MenuItem("About Dear ImGui", NULL, true);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Math Testing"))
		{
			if (ImGui::MenuItem("Random", NULL))
			{
				menuMathRandomTest = true;
			}

			if (ImGui::MenuItem("Sphere", NULL))
			{
				sphereCollisionTest = true;
			}

			if (ImGui::MenuItem("AABB", NULL))
			{
				AABBCollisionTest = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}


updateStatus M_UIManager::stopAppButton()
{
	updateStatus ret;

	ImGui::Begin("Admin Menu");

	if (ImGui::Button("Close APP"))
		ret = UPDATE_STOP;
	else
		ret = UPDATE_CONTINUE;

	ImGui::End();

	return ret;
}


