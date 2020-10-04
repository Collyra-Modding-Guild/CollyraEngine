#include "WG_Config.h"
#include "Application.h"
#include "M_Window.h"
#include "Globals.h"

#include "p2Defs.h"

WG_Config::WG_Config(bool isActive) : WindowGroup(WG_CONFIG, isActive),
fpsLog(FRAMERATE_LOG_SIZE), msLog(FRAMERATE_LOG_SIZE)
{}

WG_Config::~WG_Config()
{}

updateStatus WG_Config::Update()
{
	updateStatus ret = UPDATE_CONTINUE;

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
