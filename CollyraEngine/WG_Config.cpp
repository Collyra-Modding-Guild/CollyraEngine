#include "WG_Config.h"
#include "Application.h"
#include "M_Window.h"
#include "Globals.h"
#include "M_Renderer3D.h"

#include "p2Defs.h"

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

WG_Config::WG_Config(bool isActive) : WindowGroup(WG_CONFIG, isActive),
fpsLog(FRAMERATE_LOG_SIZE), msLog(FRAMERATE_LOG_SIZE), newInput(false), debugMode(true)
{

	
	inputModule = (M_Input*)App->GetModulePointer(M_INPUT);

	windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);
}

WG_Config::~WG_Config()
{}

updateStatus WG_Config::Update()
{
	updateStatus ret = UPDATE_CONTINUE;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Configuration");

	static char string[SHORT_STR];

	M_Window* windowModule = (M_Window*)App->GetModulePointer(M_WINDOW);

	if (windowModule == nullptr || windowModule->GetType() != M_WINDOW)
	{
		return UPDATE_STOP;
	}


	if (ImGui::CollapsingHeader("Application"))
	{
		// - - - - - - - App Title - - - - - - - - -
		strcpy_s(string, SHORT_STR, windowModule->winTitle);
		if (ImGui::InputText("App Name", string, 26))
			windowModule->SetTitle(string);

		// - - - - - - - Organization - - - - - - - - -
		strcpy_s(string, SHORT_STR, windowModule->orgTitle);
		ImGui::InputText("Organization", string, 26);

		ImGui::Separator(); // - - - - - - - - - - - - -

		// - - - - - - - - FPS Cap - - - - - - - - - -
		ImGui::SliderFloat("Limit Framerate", &App->capTime, 0, 125);
		ImGui::Text("Limit FPS: %i", int(App->capTime));

		// - - - - - - - - Framerate Stuff Graphs - - - - - - - - - -

		sprintf_s(string, SHORT_STR, "Framerate %.1f: ", fpsLog[fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fpsLog[0], 100, 0, string, 0.0f, 100.0f, ImVec2(310, 100));

		sprintf_s(string, SHORT_STR, "Miliseconds %0.1f: ", msLog[msLog.size() - 1]);
		ImGui::PlotHistogram("##miliseconds", &msLog[0], 100, 0, string, 0.0f, 40.0f, ImVec2(310, 100));

	}

	if (ImGui::CollapsingHeader("Window"))
	{
		// - - - - - - - - Window Brightness - - - - - - - - - -
		if (ImGui::SliderFloat("Brightness", &windowModule->brightness, 0.0f, 1.0f))
			SDL_SetWindowBrightness(windowModule->window, windowModule->brightness);

		// - - - - - - - - Screen Surface - - - - - - - - - -
		if (ImGui::SliderInt("Width", &windowModule->screenWidth, 0, 1920))
			SDL_SetWindowSize(windowModule->window, windowModule->screenWidth, windowModule->screenHeight);

		if (ImGui::SliderInt("Height", &windowModule->screenHeight, 0, 1080))
			SDL_SetWindowSize(windowModule->window, windowModule->screenWidth, windowModule->screenHeight);

		// - - - - - - - - Display Modes - - - - - - - - - -
		if (ImGui::Checkbox("Fullscreen", &windowModule->fullscreen))
			windowModule->SetFullscreen(windowModule->fullscreen);

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &windowModule->resizable))
			SDL_SetWindowResizable(windowModule->window, (SDL_bool)windowModule->resizable);

		if (ImGui::Checkbox("Borderless", &windowModule->borderless))
			windowModule->SetBorderless(windowModule->borderless);

		ImGui::SameLine();

		if (ImGui::Checkbox("Full Desktop", &windowModule->fullscreen_desktop))
			windowModule->SetFullscreenDesktop(windowModule->fullscreen_desktop);

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
			
		ImGui::SameLine();
		
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
	}

	ImGui::End();

	return ret;
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

bool WG_Config::IsDebugModeOn()
{
	return debugMode;
}
