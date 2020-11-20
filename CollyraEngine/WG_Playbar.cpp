#include "WG_Playbar.h"
#include "Application.h"
#include "M_Scene.h"
#include "Timer.h"

#include <string>

WG_Playbar::WG_Playbar(bool isActive) : WindowGroup(WG_PLAYBAR, isActive)
{}

WG_Playbar::~WG_Playbar()
{}

updateStatus WG_Playbar::Update()
{

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Play Bar", &active);

	std::string playButtonText = App->gameClock->running ? "Stop" : "Play";
		
	if (ImGui::Button(playButtonText.c_str()))
		App->gameClock->running ? App->scene->Stop() : App->scene->Play();

	ImGui::SameLine();

	std::string pauseButtonText = App->gameClock->running ? "Pause" : "Resume";

	if (ImGui::Button(pauseButtonText.c_str()))
		App->gameClock->running ? App->scene->Pause() : App->scene->Resume();

	ImGui::SameLine();

	std::string tickButtonText = "Advance";

	if (App->gameClock->tick) 
	{
		App->gameClock->Tick(false);
	}	

	if (ImGui::Button(tickButtonText.c_str()))
		App->scene->Tick();


	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Playbar::Cleanup()
{}