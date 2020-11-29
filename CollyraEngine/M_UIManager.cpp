#include "Application.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_Scene.h"
#include "M_Resources.h"
#include "M_FileManager.h"

#include "GameObject.h"
#include "Component.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Transform.h"

#include "Primitive.h"
#include "R_Scene.h"

#include "MathGeoLib/include/Math/float2.h"
#include <algorithm>
#include "PathNode.h"

// All UI Windows
#include "WindowGroup.h"
#include "WG_Config.h"
#include "WG_Console.h"
#include "WG_Scene.h"
#include "WG_Hierarchy.h"
#include "WG_Inspector.h"
#include "WG_About.h"
#include "WG_Playbar.h"
#include "WG_ResourceCount.h"
#include "WG_Assets.h"

//OpenGL
#include "OpenGL.h"

//ImGui
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

M_UIManager::M_UIManager(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), showDemoWindow(false), showConfigMenu(false),
configWindow(nullptr), consoleWindow(nullptr), sceneWindow(nullptr), hierarchyWindow(nullptr), inspectorWindow(nullptr),
playWindow(nullptr), aboutWindow(nullptr), resourceCount(nullptr), lastSavedId(0), showLoadScenePop(false)
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
	windowGroups.push_back(aboutWindow = new WG_About(false));
	windowGroups.push_back(playWindow = new WG_Playbar(true));
	windowGroups.push_back(resourceCount = new WG_ResourceCount(true));
	windowGroups.push_back(assetsWindow = new WG_Assets(true));

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


	// IMGUI STYLE INIT
	SetupSmoothImGuiStyle(true);

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

	for (uint i = 0; i < windowGroups.size(); i++)
	{
		if (windowGroups[i]->active == true)
		{
			windowGroups[i]->Start();
		}
	}

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

	//Pop-Ups---------------------------
	if (showLoadScenePop == true)
	{
		ShowLoadScenePopUp();
	}

	if (selectedMenuNode != "")
	{
		ShowSelectedItemPopUp();
	}


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
	playWindow = nullptr;

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
		if (ImGui::BeginMenu("Editor"))
		{
			if (ImGui::MenuItem("Load Scene", NULL))
			{
				showLoadScenePop = true;
			}

			if (ImGui::MenuItem("Save Scene", NULL))
			{
				SaveScene();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Game Objects"))
		{
			ImVec2 buttonSize = { 70, 20 };

			if (ImGui::BeginMenu("Create . . ."))
			{
				if (ImGui::Button("Empty", buttonSize))
				{
					GameObject* empty = App->scene->CreateGameObject("GameObject");
				}
				if (ImGui::Button("Child", buttonSize))
				{
					if (App->scene->focusedGameObject != nullptr)
					{
						GameObject* child = App->scene->CreateGameObject("Child");
						child->SetParent(App->scene->focusedGameObject);
					}
					else
						LOG("First you should select a GameObject.")
				}
				if (ImGui::Button("Camera", buttonSize))
				{
					GameObject* camera = App->scene->CreateGameObject("Camera");
					camera->CreateComponent(COMPONENT_TYPE::CAMERA);
				}
				if (ImGui::Button("Cube", buttonSize))
				{
					uint toLoad = App->resources->ImportResourceFromAssets(".innerAssets/cube.FBX");
					App->resources->RequestResource(toLoad);
				}
				if (ImGui::Button("Sphere", buttonSize))
				{
					uint toLoad = App->resources->ImportResourceFromAssets(".innerAssets/Sphere.FBX");
					App->resources->RequestResource(toLoad);
				}
				if (ImGui::Button("Cylinder", buttonSize))
				{
					uint toLoad = App->resources->ImportResourceFromAssets(".innerAssets/Cylinder.FBX");
					App->resources->RequestResource(toLoad);
				}
				if (ImGui::Button("Pyramid", buttonSize))
				{
					uint toLoad = App->resources->ImportResourceFromAssets(".innerAssets/Pyramid.FBX");
					App->resources->RequestResource(toLoad);
				}
				ImGui::EndMenu();

			}
			ImGui::EndMenu();

		}

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

	if (ImGui::BeginMenu("Preferences"))
	{

		if (ImGui::BeginMenu("UI Style"))
		{
			ImVec2 buttonSize = { 70, 20 };

			if (ImGui::Button("Smooth", buttonSize))
			{
				SetupSmoothImGuiStyle(true);
			}
			if (ImGui::Button("Dark", buttonSize))
			{
				SetupDarkImGuiStyle(1.0f);
			}
			if (ImGui::Button("Light", buttonSize))
			{
				SetupLightImGuiStyle();
			}


			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

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

void M_UIManager::ShowLoadScenePopUp()
{
	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::BeginChild("File Browser", ImVec2(300, 400), true);
		std::string pushedValue = DrawDirectoryRecursiveOld(LIBRARY_SCENES);
		ImGui::EndChild();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			showLoadScenePop = false;
		}

		if (pushedValue != "")
		{
			if (pushedValue.find(".collscene") < pushedValue.length())
			{
				pushedValue = pushedValue.substr(0, pushedValue.length() - 11);
				App->resources->LoadResource(std::stoi(pushedValue));
				pushedValue.clear();
			}
			showLoadScenePop = false;
		}

		ImGui::EndPopup();
	}

}

void M_UIManager::ShowSelectedItemPopUp()
{
	if (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(2))
		selectedMenuNode = "";

	if (ImGui::BeginPopup("my_select_popup"))
	{
		ImGui::Text("Aquarium");
		ImGui::Separator();

		ImGui::EndPopup();
	}
}

void M_UIManager::NewFpsLog(float currMs, float currFps)
{
	if (configWindow != nullptr)
	{
		configWindow->NewLogFramerate(currMs, currFps);
	}
}

bool* M_UIManager::GetDrawFlags()
{
	if (configWindow != nullptr)
	{
		return (configWindow->GetDrawFlags());
	}
	return nullptr;
}

void M_UIManager::GetWindowSceneSize(float& w, float& h)
{
	if (this->sceneWindow != nullptr)
	{
		this->sceneWindow->GetWindowSize(w, h);
	}
}

void M_UIManager::GameObjectDestroyed(uint id)
{
	if (inspectorWindow != nullptr)
	{
		WG_Inspector* inspector = (WG_Inspector*)inspectorWindow;
		inspector->OnDestroyedId(id);
	}

	if (hierarchyWindow != nullptr)
	{
		WG_Hierarchy* hierarchy = (WG_Hierarchy*)hierarchyWindow;
		hierarchy->OnDestroyedId(id);
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

void M_UIManager::SetFocusedGameObject(int id)
{
	if (inspectorWindow != nullptr)
	{
		WG_Inspector* inspector = (WG_Inspector*)inspectorWindow;
		inspector->SetGameObject(id);
	}

	if (hierarchyWindow != nullptr)
	{
		WG_Hierarchy* hierarchy = (WG_Hierarchy*)hierarchyWindow;
		hierarchy->NewFocusedGameObject(id);
	}
}

int M_UIManager::GetFocusedGameObjectId() const
{
	if (inspectorWindow != nullptr)
	{
		WG_Inspector* inspector = (WG_Inspector*)inspectorWindow;
		return inspector->GetFocusedGameObjectId();
	}
	return -1;
}

void M_UIManager::OnWindowResize() const
{
	if (configWindow != nullptr)
	{
		WG_Config* config = (WG_Config*)configWindow;
		return config->OnWindowResize();
	}
}

float2 M_UIManager::GetSceneWindowSize() const
{
	float w, h;

	if (sceneWindow != nullptr)
	{
		WG_Scene* scene = (WG_Scene*)sceneWindow;
		scene->GetWindowSize(w, h);
	}

	return float2({ w,h });
}

float2 M_UIManager::GetSceneWindowPosition() const
{
	float x, y;

	if (sceneWindow != nullptr)
	{
		WG_Scene* scene = (WG_Scene*)sceneWindow;
		scene->GetWindowPosition(x, y);
	}

	return float2({ x,y });
}

float2 M_UIManager::GetImGuiMousePosition() const
{
	return float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
}


void M_UIManager::SetupSmoothImGuiStyle(bool volumeEffect)
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	/// 0 = FLAT APPEARENCE
	/// 1 = MORE "3D" LOOK
	int is3D = volumeEffect;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

	style.PopupRounding = 3;

	style.WindowPadding = ImVec2(4, 4);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing = ImVec2(6, 2);

	style.ScrollbarSize = 18;

	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = is3D;

	style.WindowRounding = 3;
	style.ChildRounding = 3;
	style.FrameRounding = 3;
	style.ScrollbarRounding = 2;
	style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK 
	style.TabBorderSize = is3D;
	style.TabRounding = 3;

	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
#endif

}

void M_UIManager::SetupLightImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.95f, 0.95f, 0.30f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.66f, 0.66f, 0.66f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.000f, 0.777f, 0.578f, 0.780f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.987f, 0.611f, 0.600f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.77f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.84f, 0.97f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.86f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(1.00f, 0.54f, 0.01f, 0.71f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.96f, 0.73f, 0.09f, 0.90f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.97f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);


}

uint M_UIManager::SaveScene()
{
	return App->resources->SaveResource((Resource*)App->scene->GetSceneResource(), "", false);
}

void M_UIManager::SetupDarkImGuiStyle(float alpha)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

std::string M_UIManager::DrawDirectoryRecursiveOld(const char* directory, bool returnFullPath, std::vector<std::string>* ignoreExt, char* dragType)
{
	std::string dir((directory) ? directory : "");
	if (dir.find_last_of("/") == dir.length() - 1)
		dir = dir.substr(0, dir.length() - 1);

	std::string ret = "";

	PathNode* myPathNode;
	PathNode createdPathNode;

	if (directory == ASSETS_FOLDER)
	{
		myPathNode = App->resources->GetAllAssetFiles();
	}
	else
	{
		createdPathNode = App->physFS->GetAllFiles(directory, nullptr, ignoreExt);
		myPathNode = &createdPathNode;
	}

	for (uint i = 0; i < myPathNode->children.size(); i++)
	{
		ret = DrawDirectoryRecursive(&myPathNode->children.at(i), directory, returnFullPath, ignoreExt, dragType);

		if (ret != "")
			break;
	}

	return ret;
}

std::string M_UIManager::DrawDirectoryRecursive(PathNode* myNode, const char* directory, bool returnFullPath, std::vector<std::string>* ignoreExt, char* dragType)
{
	std::string ret = "";

	if (!myNode->isFile)
	{
		if (ImGui::TreeNodeEx((myNode->path).c_str(), 0, "%s/", myNode->path.c_str()))
		{
			if (!myNode->isLeaf)
			{
				for (uint i = 0; i < myNode->children.size(); i++)
				{
					ret = DrawDirectoryRecursive(&myNode->children.at(i), directory, returnFullPath, ignoreExt, dragType);
					if (ret != "")
					{
						break;
						return ret;
					}
				}
			}
			ImGui::TreePop();
			if (ret != "")
			{
				return ret;
			}

		}
	}
	else
	{
		const std::string& str = myNode->localPath;

		bool ignore = false;
		if (ignoreExt != nullptr)
		{
			ignore = App->physFS->HasExtension(myNode->path.c_str(), *ignoreExt);
		}

		if (ignore == false)
		{
			if (ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
			{
				if (ImGui::IsItemClicked() || ImGui::IsItemClicked(2))
				{
					if (ImGui::IsMouseDoubleClicked(0) || ImGui::IsItemClicked(2))
					{
						ret = str;
					}
				}

				if (dragType != nullptr)
				{
					if (ImGui::BeginDragDropSource())
					{
						std::string treeNode = str;
						if (treeNode != "")
						{
							if (returnFullPath)
								treeNode = myNode->path.c_str();
						}

						ImGui::SetDragDropPayload(dragType, treeNode.c_str(), treeNode.length());

						ImGui::Text("%s", treeNode.c_str());

						ImGui::EndDragDropSource();
					}
				}


				ImGui::TreePop();

				if (ret != "")
				{
					if (returnFullPath)
						return myNode->path.c_str();
				}
			}
		}
	}


	return ret;
}
