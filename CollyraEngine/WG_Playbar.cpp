#include "WG_Playbar.h"
#include "Application.h"

#include <string>

WG_Playbar::WG_Playbar(bool isActive) : WindowGroup(WG_PLAYBAR, isActive)
{}

WG_Playbar::~WG_Playbar()
{}

updateStatus WG_Playbar::Update()
{

	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Play Bar", &active);

	std::string playButtonText = "Play";
		
	if (ImGui::Button(playButtonText.c_str()))
	{
		// PLAY stuff
	}

	ImGui::SameLine();

	std::string StopButtonText = "Stop";

	if (ImGui::Button(StopButtonText.c_str()))
	{
		// STOP stuff
	}

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Playbar::Cleanup()
{}