#include "Globals.h"
#include "Application.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "M_Input.h"
#include "M_Window.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

#pragma comment (lib, "Glew/libx86/glew32.lib")


M_UIManager::M_UIManager(Application* app, bool start_enabled) : Module(app, start_enabled), showDemoWindow(false), menuMathRandomTest(false), generateRandomNumbers(false), generatedInt(0), generatedFloat(0.0f),
fps_log(100), ms_log(100), sphereCollisionTest(false), AABBCollisionTest(false), OBBCollisionTest(false), randomStartThreshold(0.f), randomEndThreshold(0.f), planeCollisionTest(false),
rayCollisionTest(false), triangleCollisionTest(false), logInputs()
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

	AABB1 = AABB({ 0.f, 0.f, 0.f }, { 10.f, 10.f, 10.f });
	AABB1 = AABB({ 0.f, 0.f, 0.f }, { 5.f, 5.f, 5.f });


	OBB1 = OBB();
	OBB1.SetFrom(AABB1, float3x3().FromEulerXYZ(0, 0, 0));

	OBB2 = OBB();
	OBB2.SetFrom(AABB1, float3x3().FromEulerXYZ(90, 0, 0));

	plane1 = Plane({ 1,1,1 }, 1.f);
	plane2 = Plane({ 1,1,1 }, 0.f);

	ray1 = Ray({ 0.f,0.f,0.f }, { 1.f, 0.f, 0.f });
	ray2 = Ray({ 20.f,0.f,0.f }, { -1.f, 0.f, 0.f });

	tri1 = Triangle({ 0.f,0.f,0.f }, { 5.f,5.f,5.f }, { 10.f,10.f,10.f });
	tri2 = Triangle({ 10.f,10.f,10.f }, { 5.f,5.f,5.f }, { 0.f,0.f,0.f });

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

	ImGuiIO& io = ImGui::GetIO();

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

		CalculateHistogramLogs();

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
			SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->resizable);

		if (ImGui::Checkbox("Borderless", &App->window->borderless))
			App->window->SetBorderless(App->window->borderless);

		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &App->window->fullscreen_desktop))
			App->window->SetFullscreenDesktop(App->window->fullscreen_desktop);

	}

	if (ImGui::CollapsingHeader("File System"))
	{
		ImGui::Text("In work process . . .");
	}

	if (ImGui::CollapsingHeader("Input"))
	{

		// - - - - - - - - Mouse Reading - - - - - - - - -
		ImGui::Text("Mouse Position:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i, %i", App->input->GetMouseX(), App->input->GetMouseY());

		ImGui::Text("Mouse Motion:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i, %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());

		ImGui::Text("Mouse Wheel:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i", App->input->GetMouseZ());

		// - - - - - - - - General Input Reading - - - - - - - - -

		ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
		ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
		ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
		ImGui::Text("Mouse dblclick:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
		ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }

		ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
		ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
		ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
		ImGui::SameLine();
		if (ImGui::Button("Clear Input Log"))
		{
			logInputs.clear();
		}

		ImGui::Separator();

		ImGui::BeginChild("Inputs Log");
		ImGui::TextUnformatted(logInputs.begin());
		ImGui::EndChild();

	}

	if (ImGui::CollapsingHeader("Hardware"))
	{

		// - - - - - - - - Hardware/Software Specs - - - - - - - - -
		SDL_version ver;
		SDL_GetVersion(&ver);

		ImGui::Text("SDL Version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%d.%d.%d", ver.major, ver.minor, ver.patch);

		ImGui::Separator();

		ImGui::Text("CPUs:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i Cores", SDL_GetCPUCount());
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "(Cache: %ikb)", SDL_GetCPUCacheLineSize());

		ImGui::Text("System RAM:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%.1fGb", (SDL_GetSystemRAM() / 1024.f));

		ImGui::Text("Caps:"); ImGui::SameLine();
		if (SDL_HasRDTSC()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "RDTSC,"); ImGui::SameLine();
		if (SDL_HasMMX()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "MMX,");  ImGui::SameLine();
		if (SDL_Has3DNow()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "3D,"); ImGui::SameLine();
		if (SDL_HasSSE()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "SSE,"); ImGui::SameLine();
		if (SDL_HasSSE2()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "SSE2,");

		if (SDL_HasSSE3()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "SSE3,"); ImGui::SameLine();
		if (SDL_HasSSE41()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "SSE41,"); ImGui::SameLine();
		if (SDL_HasSSE42()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "SSE42"); ImGui::SameLine();
		if (SDL_HasAltiVec()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "AltiVec,"); ImGui::SameLine();
		if (SDL_HasAVX()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "AVX,"); ImGui::SameLine();
		if (SDL_HasAVX2()) ImGui::TextColored({ 255 , 255 , 0 , 100 }, "AVX2.");

		ImGui::Separator();

		// We will need to get the GPU info as a bonus work.
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

	if (OBBCollisionTest)
		ShowMenuOBBCollisions();

	if (planeCollisionTest)
		ShowMenuPlaneCollisions();

	if (rayCollisionTest)
		ShowMenuRayCollisions();

	if (triangleCollisionTest)
		ShowMenuTriangleCollisions();

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

	logInputs.clear();

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


	if (ImGui::Button("Generate New AABBs"))
	{
		AABB1.minPoint.x = randomSeed.Float(0, 5);
		AABB1.minPoint.y = randomSeed.Float(0, 5);
		AABB1.minPoint.z = randomSeed.Float(0, 5);

		AABB1.maxPoint.x = randomSeed.Float(5, 10);
		AABB1.maxPoint.y = randomSeed.Float(5, 10);
		AABB1.maxPoint.z = randomSeed.Float(5, 10);

		AABB2.minPoint.x = randomSeed.Float(5, 10);
		AABB2.minPoint.y = randomSeed.Float(5, 10);
		AABB2.minPoint.z = randomSeed.Float(5, 10);

		AABB2.maxPoint.x = randomSeed.Float(10, 15);
		AABB2.maxPoint.y = randomSeed.Float(10, 15);
		AABB2.maxPoint.z = randomSeed.Float(10, 15);

	}

	ImGui::Text("AABB 1 \n MinSize: %.2f, %f, %.2f \n MaxSize: %.2f, %f, %.2f", AABB1.minPoint.x, AABB1.minPoint.y, AABB1.minPoint.z,
		AABB1.maxPoint.x, AABB1.maxPoint.y, AABB1.maxPoint.z);

	ImGui::Text("AABB 2 \n MinSize: %.2f, %f, %.2f \n MaxSize: %.2f, %f, %.2f", AABB2.minPoint.x, AABB2.minPoint.y, AABB2.minPoint.z,
		AABB2.maxPoint.x, AABB2.maxPoint.y, AABB2.maxPoint.z);

	ImGui::NewLine();

	if (AABB1.Intersection(AABB2).Volume() > 0)
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}

bool M_UIManager::ShowMenuOBBCollisions()
{
	if (!ImGui::Begin("Random Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}

	if (ImGui::Button("Generate New OBBs"))
	{
		OBB1.pos.x = randomSeed.Float(0, 5);
		OBB1.pos.y = randomSeed.Float(0, 5);
		OBB1.pos.z = randomSeed.Float(0, 5);

		OBB1.r.x = randomSeed.Float(1, 2.5);
		OBB1.r.y = randomSeed.Float(1, 2.5);
		OBB1.r.z = randomSeed.Float(1, 2.5);

		OBB1.Transform(float3x3().FromEulerXYZ(randomSeed.Float(0, 359), randomSeed.Float(0, 359), randomSeed.Float(0, 359)));

		OBB2.pos.x = randomSeed.Float(0, 5);
		OBB2.pos.y = randomSeed.Float(0, 5);
		OBB2.pos.z = randomSeed.Float(0, 5);

		OBB2.r.x = randomSeed.Float(1, 2.5);
		OBB2.r.y = randomSeed.Float(1, 2.5);
		OBB2.r.z = randomSeed.Float(1, 2.5);

		OBB2.Transform(float3x3().FromEulerXYZ(randomSeed.Float(0, 359), randomSeed.Float(0, 359), randomSeed.Float(0, 359)));

	}

	ImGui::Text("OBB 1 \n Center: %.2f, %f, %.2f \n Radius: %.2f, %f, %.2f \n AxisX: %.2f, %.2f, %.2f\n AxisY: %.2f, %.2f, %.2f \n AxisZ: %.2f, %.2f, %.2f",
		OBB1.pos.x, OBB1.pos.y, OBB1.pos.z,
		OBB1.r.x, OBB1.r.y, OBB1.r.z,
		OBB1.axis[0].x, OBB1.axis[0].y, OBB1.axis[0].z,
		OBB1.axis[1].x, OBB1.axis[1].y, OBB1.axis[1].z,
		OBB1.axis[2].x, OBB1.axis[2].y, OBB1.axis[2].z);


	ImGui::Text("OBB 2 \n Center: %.2f, %f, %.2f \n Radius: %.2f, %f, %.2f \n AxisX: %.2f, %.2f, %.2f\n AxisY: %.2f, %.2f, %.2f \n AxisZ: %.2f, %.2f, %.2f",
		OBB2.pos.x, OBB2.pos.y, OBB2.pos.z,
		OBB2.r.x, OBB2.r.y, OBB2.r.z,
		OBB2.axis[0].x, OBB2.axis[0].y, OBB2.axis[0].z,
		OBB2.axis[1].x, OBB2.axis[1].y, OBB2.axis[1].z,
		OBB2.axis[2].x, OBB2.axis[2].y, OBB2.axis[2].z);


	ImGui::NewLine();


	if (OBB1.Intersects(OBB2))
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}

bool M_UIManager::ShowMenuPlaneCollisions()
{
	if (!ImGui::Begin("Random Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}

	if (ImGui::Button("Generate New Planes"))
	{
		plane1.d = randomSeed.Float(0, 10);
		plane1.normal = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();

		plane2.d = randomSeed.Float(0, 10);
		plane2.normal = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();

	}

	if (ImGui::Button("Generate parallel Planes"))
	{
		plane1.d = randomSeed.Float(0, 10);
		plane1.normal = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();

		plane2.d = randomSeed.Float(0, 10);
		plane2.normal = plane1.normal;

	}

	ImGui::Text("Plane 1 \n Dir: %.2f, %f, %.2f \n Offset: %.2f",
		plane1.normal.x, plane1.normal.y, plane1.normal.z,
		plane1.d);


	ImGui::Text("Plane 2 \n Dir: %.2f, %f, %.2f \n Offset: %.2f",
		plane2.normal.x, plane2.normal.y, plane2.normal.z,
		plane2.d);


	ImGui::NewLine();


	if (plane1.Intersects(plane2))
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}

bool M_UIManager::ShowMenuRayCollisions()
{
	if (!ImGui::Begin("Random Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}

	if (ImGui::Button("Generate New Rays"))
	{

		ray1.dir = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();
		ray1.pos = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));

		ray2.dir = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();
		ray2.pos = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));

	}

	if (ImGui::Button("Generate Intersectable Rays"))
	{
		ray1.dir = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10)).Normalized();
		ray1.pos = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));

		ray2.dir = ray1.dir * -1;
		ray2.pos = ray1.pos + ray1.dir * randomSeed.Float(0, 10);

	}

	ImGui::Text("Ray 1 \n Dir: %.2f, %f, %.2f \n Pos: %.2f, %f, %.2f",
		ray1.dir.x, ray1.dir.y, ray1.dir.z,
		ray1.pos.x, ray1.pos.y, ray1.pos.z);


	ImGui::Text("Ray 1 \n Dir: %.2f, %f, %.2f \n Pos: %.2f, %f, %.2f",
		ray2.dir.x, ray2.dir.y, ray2.dir.z,
		ray2.pos.x, ray2.pos.y, ray2.pos.z);


	ImGui::NewLine();

	float rayDistance = ray1.Distance(ray2);


	if (rayDistance < 0.0001f)
	{
		ImGui::Text("Intersection Detected!");
	}
	else
	{
		ImGui::Text("No Intersection Detected :(");
	}

	ImGui::End();
}

bool M_UIManager::ShowMenuTriangleCollisions()
{
	if (!ImGui::Begin("Triangle Menu", &menuMathRandomTest))
	{
		ImGui::End();
		return false;
	}

	if (ImGui::Button("Generate New Tris"))
	{

		tri1.a = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));
		tri1.b = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));
		tri1.c = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));

		tri2.a = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));
		tri2.b = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));
		tri2.c = float3(randomSeed.Float(0, 10), randomSeed.Float(0, 10), randomSeed.Float(0, 10));

	}

	ImGui::Text("Tri 1 \n Point 1: %.2f, %f, %.2f \n Point 2: %.2f, %f, %.2f \n Point 3: %.2f, %f, %.2f",
		tri1.a.x, tri1.a.y, tri1.a.z,
		tri1.b.x, tri1.b.y, tri1.b.z,
		tri1.c.x, tri1.c.y, tri1.c.z);

	ImGui::Text("Tri 1 \n Point 1: %.2f, %f, %.2f \n Point 2: %.2f, %f, %.2f \n Point 3: %.2f, %f, %.2f",
		tri2.a.x, tri2.a.y, tri2.a.z,
		tri2.b.x, tri2.b.y, tri2.b.z,
		tri2.c.x, tri2.c.y, tri2.c.z);


	ImGui::NewLine();

	if (tri1.Intersects(tri2))
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

			if (ImGui::MenuItem("OBB", NULL))
			{
				OBBCollisionTest = true;
			}

			if (ImGui::MenuItem("Plane", NULL))
			{
				planeCollisionTest = true;
			}

			if (ImGui::MenuItem("Rays", NULL))
			{
				rayCollisionTest = true;
			}

			if (ImGui::MenuItem("Triangles", NULL))
			{
				triangleCollisionTest = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void M_UIManager::CalculateHistogramLogs()
{
	static int logSize = 0;

	if (logSize == 100)
		for (int i = 0; i < 100 - 1; ++i) {
			ms_log[i] = ms_log[i + 1];
			fps_log[i] = fps_log[i + 1];
		}
	else
		logSize++;

	ms_log[logSize - 1] = App->last_frame_ms;
	fps_log[logSize - 1] = App->frames_on_last_update;

}


void M_UIManager::NewInputLog(uint key, uint state, bool isMouse)
{
	static const char* currState = nullptr;
	switch (state)
	{
	case(KEY_DOWN):
	{
		currState = "DOWN";
	}
	break;
	case(KEY_REPEAT):
	{
		currState = "REPEAT";
	}
	break;
	case(KEY_UP):
	{
		currState = "UP";
	}
	break;
	}

	if (!isMouse)
	{
		logInputs.appendf("Key: %s - %s\n", SDL_GetScancodeName(SDL_Scancode(key)), currState);
	}
	else
	{
		logInputs.appendf("Mouse: %02u - %s\n", key, currState);
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

