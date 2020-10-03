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

M_UIManager::M_UIManager(Application* app, bool start_enabled) : Module(app, start_enabled), showDemoWindow(false)
{}

// Destructor
M_UIManager::~M_UIManager()
{}

// Called before render is available
bool M_UIManager::Awake()
{
	showDemoWindow = false;

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

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (showDemoWindow)
        ImGui::ShowDemoWindow(&showDemoWindow);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Configuration");

	if (ImGui::CollapsingHeader("Application"))
	{
		static char* winTitle;
		winTitle = TITLE;

		if (ImGui::InputText(" App Name", winTitle, 40))
		{
			App->window->SetTitle(winTitle);
		}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::SliderFloat("Brightness", &f, 0.0f, 1.0f);
		ImGui::SliderFloat("Width", &f, 0, 1280);
		ImGui::SliderFloat("Height", &f, 0, 1024);
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
        ImGui::EndMainMenuBar();
    }


	return stopAppButton();
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


	