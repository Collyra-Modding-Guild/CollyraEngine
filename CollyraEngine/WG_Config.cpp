#include "WG_Config.h"
#include "Application.h"
#include "M_Window.h"
#include "M_Renderer3D.h"
#include "M_Input.h"

#include "p2Defs.h"

//OpenGL
#include "OpenGL.h"

//ImGui
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL.h"

//DevIL
#include "Devil/include/il.h"

WG_Config::WG_Config(bool isActive) : WindowGroup(WG_CONFIG, isActive),
fpsLog(FRAMERATE_LOG_SIZE), msLog(FRAMERATE_LOG_SIZE), newInput(false), drawFlags{true, false, false, false, false, true, false, false},
buffWinW(0), buffWinH(0)
{}

WG_Config::~WG_Config()
{}

updateStatus WG_Config::Start()
{
	OnWindowResize();

	return UPDATE_CONTINUE;
}

updateStatus WG_Config::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Configuration");

	static char string[SHORT_STR];

	if (ImGui::CollapsingHeader("Application"))
	{
		// - - - - - - - App Title - - - - - - - - -
		strcpy_s(string, SHORT_STR, App->window->winTitle);
		if (ImGui::InputText("App Name", string, 26))
			App->window->SetTitle(string);

		// - - - - - - - Organization - - - - - - - - -
		strcpy_s(string, SHORT_STR, App->window->orgTitle);
		ImGui::InputText("Organization", string, 26);

		ImGui::Separator(); // - - - - - - - - - - - - -

		// - - - - - - - - FPS Cap - - - - - - - - - -
		ImGui::SliderFloat("Limit Framerate", &App->capTime, 0, 125);
		ImGui::Text("Limit FPS: %i", int(App->capTime));
		ImGui::SameLine();
		M_Renderer3D* renderModule = (M_Renderer3D*)App->GetModulePointer(M_RENDER3D);

		if (renderModule == nullptr)
			return UPDATE_STOP;

		bool vSync = renderModule->GetVSync();
		if (ImGui::Checkbox("VSync", &vSync))
		{
			SDL_GL_SetSwapInterval(vSync);
		}

		renderModule->SetVSync(vSync);

		// - - - - - - - - Framerate Stuff Graphs - - - - - - - - - -

		sprintf_s(string, SHORT_STR, "Framerate %.1f: ", fpsLog[fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fpsLog[0], 100, 0, string, 0.0f, 100.0f, ImVec2(310, 100));

		sprintf_s(string, SHORT_STR, "Miliseconds %0.1f: ", msLog[msLog.size() - 1]);
		ImGui::PlotHistogram("##miliseconds", &msLog[0], 100, 0, string, 0.0f, 40.0f, ImVec2(310, 100));

	}

	if (ImGui::CollapsingHeader("Window"))
	{
		// - - - - - - - - Window Brightness - - - - - - - - - -
		if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.0f, 1.0f))
			SDL_SetWindowBrightness(App->window->window, App->window->brightness);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		// - - - - - - - - Screen Surface - - - - - - - - - -
		ImGui::SliderInt("Width", &buffWinW, 0, 1920);
		ImGui::SliderInt("Height", &buffWinH, 0, 1080);
		ImGui::Spacing();

		if (ImGui::Button("Set new Window Size"))
		{
			SDL_SetWindowSize(App->window->window, buffWinW, buffWinH);
			App->window->OnResize();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// - - - - - - - - Display Modes - - - - - - - - - -
		if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
			App->window->SetFullscreen(App->window->fullscreen);

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &App->window->resizable))
			SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->resizable);

		if (ImGui::Checkbox("Borderless", &App->window->borderless))
			App->window->SetBorderless(App->window->borderless);

		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &App->window->fullscreenDesktop))
			App->window->SetFullscreenDesktop(App->window->fullscreenDesktop);

	}

	if (ImGui::CollapsingHeader("Input"))
	{
		M_Input* inputModule = (M_Input*)App->GetModulePointer(M_INPUT);

		if (inputModule == nullptr)
			return UPDATE_STOP;

		// - - - - - - - - Mouse Reading - - - - - - - - -
		ImGui::Text("Mouse Position:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i, %i", inputModule->GetMouseX(), inputModule->GetMouseY());

		ImGui::Text("Mouse Motion:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i, %i", inputModule->GetMouseXMotion(), inputModule->GetMouseYMotion());

		ImGui::Text("Mouse Wheel:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%i", inputModule->GetMouseZ());

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
		if (newInput)
		{
			ImGui::SetScrollHere(1.0f);
			newInput = false;
		}
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

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		ImGui::Text("OpenGL Version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%d.%d", major, minor);
		
		ImGui::Text("DevIL Version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%d", IL_VERSION);

		ImGui::Text("Glew Version:");
		ImGui::SameLine();
		ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);

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


	if (ImGui::CollapsingHeader("Render Options"))
	{
		// - - - - - - - - Render Additional Options - - - - - - - - - -

		ImGui::BulletText("General:");

		bool check = glIsEnabled(GL_DEPTH_TEST);

		if (ImGui::Checkbox("Depth Test", &check))
		{
			if (glIsEnabled(GL_DEPTH_TEST))
			{
				glDisable(GL_DEPTH_TEST);
				LOG("Depth Test: DISABLED.");
			}
			else
			{
				glEnable(GL_DEPTH_TEST);
				LOG("Depth Test: ENABLED.");
			}
		}

		ImGui::SameLine();

		check = glIsEnabled(GL_CULL_FACE);

		if (ImGui::Checkbox("Cull Face", &check))
		{
			if (glIsEnabled(GL_CULL_FACE))
			{
				glDisable(GL_CULL_FACE);
				LOG("Cull Face: DISABLED.");
			}
			else
			{
				glEnable(GL_CULL_FACE);
				LOG("Cull Face: ENABLED.");
			}
		}
		ImGui::SameLine();

		check = glIsEnabled(GL_BLEND);

		if (ImGui::Checkbox("Blend", &check))
		{
			if (glIsEnabled(GL_BLEND))
			{
				glDisable(GL_BLEND);
				LOG("Blend: DISABLED.");
			}
			else
			{
				glEnable(GL_BLEND);
				LOG("Blend: ENABLED.");
			}
		}

		ImGui::BulletText("Lighting:");

		check = glIsEnabled(GL_LIGHTING);

		if (ImGui::Checkbox("Lighting", &check))
		{
			if (glIsEnabled(GL_LIGHTING))
			{
				glDisable(GL_LIGHTING);
				LOG("Lighting: DISABLED.");
			}
			else
			{
				glEnable(GL_LIGHTING);
				LOG("Lighting: ENABLED.");
			}
		}

		ImGui::BulletText("Material:");

		check = glIsEnabled(GL_COLOR_MATERIAL);

		if (ImGui::Checkbox("Color Material", &check))
		{
			if (glIsEnabled(GL_COLOR_MATERIAL))
			{
				glDisable(GL_COLOR_MATERIAL);
				LOG("Color Material: DISABLED.");
			}
			else
			{
				glEnable(GL_COLOR_MATERIAL);
				LOG("Color Material: ENABLED.");
			}
		}
		ImGui::SameLine();

		check = glIsEnabled(GL_TEXTURE_2D);

		if (ImGui::Checkbox("Texture 2D", &check))
		{
			if (glIsEnabled(GL_TEXTURE_2D))
			{
				glDisable(GL_TEXTURE_2D);
				LOG("Texture 2D: DISABLED.");
			}
			else
			{
				glEnable(GL_TEXTURE_2D);
				LOG("Texture 2D: ENABLED.");
			}
		}


		ImGui::BulletText("Draw Modes:");

		ImGui::Checkbox("Wire Draw", &drawFlags[WIRE]);
		ImGui::SameLine();
		ImGui::Checkbox("Draw Faces", &drawFlags[FACES]);

		ImGui::BulletText("Draw Normals:");

		ImGui::Checkbox("Vertex Normals", &drawFlags[NORMAL_V]);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFlags[NORMAL_F]);


		ImGui::BulletText("Others:");

		ImGui::Checkbox("Bounding Box", &drawFlags[BOUNDING_BOX]);
		ImGui::SameLine();
		ImGui::Checkbox("Frustum", &drawFlags[FRUSTUM]);

		ImGui::Checkbox("Mouse Ray", &drawFlags[MOUSE_RAY]);
		ImGui::SameLine();
		ImGui::Checkbox("Outline", &drawFlags[OUTLINE]);

	}

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Config::Cleanup()
{
	logInputs.clear();

	fpsLog.clear();
	msLog.clear();
}

void WG_Config::NewLogInput(const char* newInputLog)
{
	logInputs.append(newInputLog);
	newInput = true;

}

void WG_Config::NewLogFramerate(float newMs, float newFps)
{
	static int logSize = 0;

	if (logSize == FRAMERATE_LOG_SIZE)
	{
		for (int i = 0; i < FRAMERATE_LOG_SIZE - 1; ++i)
		{
			msLog[i] = msLog[i + 1];
			fpsLog[i] = fpsLog[i + 1];
		}
	}
	else
	{
		logSize++;
	}

	msLog[logSize - 1] = newMs;
	fpsLog[logSize - 1] = newFps;

}

bool* WG_Config::GetDrawFlags()
{
	return drawFlags;
}

void WG_Config::OnWindowResize()
{
	buffWinW = App->window->screenWidth;
	buffWinH = App->window->screenHeight;
}
