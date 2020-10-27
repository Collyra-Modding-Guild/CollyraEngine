#include "Application.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "M_Input.h"
#include "M_Window.h"


// All UI Windows
#include "WindowGroup.h"
#include "WG_Config.h"
#include "WG_Console.h"
#include "WG_Scene.h"
#include "WG_Hierarchy.h"
#include "WG_Inspector.h"
#include "WG_About.h"

//OpenGL
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

//ImGui
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

#pragma comment (lib, "Glew/libx86/glew32.lib")


M_UIManager::M_UIManager(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), showDemoWindow(false), showConfigMenu(false),
configWindow(nullptr), consoleWindow(nullptr), sceneWindow()
{}

// Destructor
M_UIManager::~M_UIManager()
{}

// Called before render is available
bool M_UIManager::Awake()
{
	//Window Groups-------------
	windowGroups.push_back(configWindow = new WG_Config(true));
	windowGroups.push_back(consoleWindow = new WG_Console(true));
	windowGroups.push_back(sceneWindow = new WG_Scene(true));
	windowGroups.push_back(hierarchyWindow = new WG_Hierarchy(true));
	windowGroups.push_back(inspectorWindow = new WG_Inspector(true));
	windowGroups.push_back(aboutWindow = new WG_About(true));

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

	M_Window* windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);
	if (windowModule == nullptr)
	{
		return false;
	}

	M_Renderer3D* renderModule = (M_Renderer3D*)App->GetModulePointer(M_RENDER3D);
	if (renderModule == nullptr)
	{
		return false;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(windowModule->window, renderModule->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

updateStatus M_UIManager::PreUpdate(float dt)
{
	updateStatus ret = UPDATE_CONTINUE;

	M_Window* windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);
	if (windowModule == nullptr)
	{
		return UPDATE_STOP;
	}

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(windowModule->window);
	ImGui::NewFrame();

	EnableDockSpace();

	return ret;
}

updateStatus M_UIManager::Update(float dt)
{
	updateStatus ret = UPDATE_CONTINUE;

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);


	for (uint i = 0; i < windowGroups.size(); i++)
	{
		if (windowGroups[i]->active == true)
		{
			ret = windowGroups[i]->Update();
		}
	}

	//-------------------------

	ret = updateStatus(ShowMainMenuBar());


	return ret;
}

// Called before quitting
bool M_UIManager::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (uint i = 0; i < windowGroups.size(); i++)
	{
		windowGroups[i]->CleanUp();
		windowGroups[i] = nullptr;
		RELEASE(windowGroups[i]);
	}

	configWindow = nullptr;
	consoleWindow = nullptr;
	sceneWindow = nullptr;
	inspectorWindow = nullptr;
	hierarchyWindow = nullptr;
	aboutWindow = nullptr;

	return true;
}

updateStatus M_UIManager::Draw2D(float dt)
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

bool M_UIManager::ShowMainMenuBar()
{
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("App"))
		{
			if (ImGui::MenuItem("Exit", NULL))
			{
				ret = false;
			}

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

		if (ImGui::BeginMenu("Primitives"))
		{
			ImVec2 buttonSize = { 70, 20 };

			if (ImGui::Button("Cube", buttonSize))
			{

			}
			if (ImGui::Button("Sphere", buttonSize))
			{

			}
			if (ImGui::Button("Cylinder", buttonSize))
			{

			}
			if (ImGui::Button("Pyramid", buttonSize))
			{

			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", NULL, true);
			ImGui::MenuItem("Style Editor", NULL, true);
			ImGui::MenuItem("About Dear ImGui", NULL, true);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			if (configWindow != nullptr)
			{
				ImGui::MenuItem("Configuration", "", &configWindow->active);
			}
			if (consoleWindow != nullptr)
			{
				ImGui::MenuItem("Console", "", &consoleWindow->active);
			}
			if (hierarchyWindow != nullptr)
			{
				ImGui::MenuItem("Hierarchy", "", &hierarchyWindow->active);
			}
			if (inspectorWindow != nullptr)
			{
				ImGui::MenuItem("Inspector", "", &inspectorWindow->active);
			}
			if (aboutWindow != nullptr)
			{
				ImGui::MenuItem("About", "", &aboutWindow->active);
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return ret;
}

void M_UIManager::EnableDockSpace()
{
	static bool optFullscreen = true;
	static bool optPadding = false;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (optFullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!optPadding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace Demo", NULL, windowFlags);
	if (!optPadding)
		ImGui::PopStyleVar();

	if (optFullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}
	else
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
		ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
		ImGui::SameLine(0.0f, 0.0f);
		if (ImGui::SmallButton("click here"))
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	ImGui::End();

}

void M_UIManager::NewFpsLog(float currMs, float currFps)
{
	if (configWindow != nullptr)
	{
		configWindow->NewLogFramerate(currMs, currFps);
	}
}

bool M_UIManager::IsDebugModeOn()
{
	if (configWindow != nullptr)
	{
		return (configWindow->IsDebugModeOn());
	}
	return false;
}

void M_UIManager::GetWindowSceneSize(float& w, float& h)
{
	if (this->sceneWindow != nullptr)
	{
		this->sceneWindow->GetWindowSize(w,h);
	}
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

	static char newInputLog[MID_STR];

	if (!isMouse)
	{
		sprintf_s(newInputLog, MID_STR, "Key: %s - %s\n", SDL_GetScancodeName(SDL_Scancode(key)), currState);
	}
	else
	{
		sprintf_s(newInputLog, MID_STR, "Mouse: %02u - %s\n", key, currState);
	}

	if (configWindow != nullptr)
	{
		configWindow->NewLogInput(newInputLog);
	}

}

void M_UIManager::NewConsoleLog(const char* newLog)
{
	if (this->consoleWindow != nullptr)
	{
		consoleWindow->NewLog(newLog);
	}
}
