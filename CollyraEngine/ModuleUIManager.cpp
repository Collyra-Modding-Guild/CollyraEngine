#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleUIManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"


#pragma comment (lib, "Glew/libx86/glew32.lib")

ModuleUIManager::ModuleUIManager(Application* app, bool start_enabled) : Module(app, start_enabled), showDemoWindow(false), showDemoWindow2(false)
{}

// Destructor
ModuleUIManager::~ModuleUIManager()
{}

// Called before render is available
bool ModuleUIManager::Init()
{
	showDemoWindow = true;
	showDemoWindow2 = true;

    glewInit();

	return true;
}

bool ModuleUIManager::Start()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 130");

    return true;
}

update_status ModuleUIManager::PreUpdate(float dt)
{
    /*
	ImGui_ImplOpenGL3_NewFrame();
    
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(&showDemoWindow);
    */
    

	return UPDATE_CONTINUE;
}

update_status ModuleUIManager::PostUpdate(float dt)
{
    /*
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &showDemoWindow);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (showDemoWindow2)
    {
        ImGui::Begin("Another Window", &showDemoWindow2);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            showDemoWindow2 = false;
        ImGui::End();
    }

    ImGui::Render();
    */
   
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleUIManager::CleanUp()
{
    /*
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    */

	return true;
}


	