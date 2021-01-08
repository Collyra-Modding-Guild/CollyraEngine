#include "WG_Playbar.h"
#include "Application.h"
#include "M_Scene.h"
#include "Timer.h"
#include "M_Renderer3D.h"

#include "GameObject.h"
#include "C_Camera.h"

#include <string>

WG_Playbar::WG_Playbar(bool isActive) : WindowGroup(WG_PLAYBAR, isActive), playCam(nullptr), playCamGameObjId(0)
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

	std::string pauseButtonText = App->gameClock->pause ? "Resume" : "Pause";

	if (ImGui::Button(pauseButtonText.c_str()) && App->gameClock->running)
		App->gameClock->pause ? App->scene->Resume() : App->scene->Pause();
	else if(!App->gameClock->running)
		App->scene->Pause();


	ImGui::SameLine();

	std::string tickButtonText = "Advance";

	if (App->gameClock->tick)
		App->gameClock->Tick(false);

	if (ImGui::Button(tickButtonText.c_str()) && App->gameClock->pause)
		App->scene->Tick();

	ShowTimeMultiplier();

	ImGui::SameLine();

	if (playCamGameObjId != 0 && playCam == nullptr)
	{
		RefreshPlayCam();
	}

	ImGui::Text("Play Camera: ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(110.0f);
	if (ImGui::BeginCombo("##currentCam", playCam ? playCam->GetGameObject()->GetName().c_str() : "")) // Check TAGS list and selected Tag
	{
		for (int n = 0; n < App->scene->cameras.size(); n++)
		{
			if (ImGui::Selectable(std::string(App->scene->cameras[n]->GetGameObject()->GetName()).append("_").append(std::to_string(n)).c_str(), true))
			{
				playCam = App->scene->cameras[n];
				playCamGameObjId = playCam->GetGameObject()->GetUid();

				if (App->gameClock->IsPlaying() == true)
				{
					App->renderer3D->SetPlayCam(playCam);
				}

			}

		}
		ImGui::EndCombo();
	}

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Playbar::Cleanup()
{
	playCam = nullptr;
}


void WG_Playbar::OnDestroyedId(unsigned int id)
{
	if (playCam != nullptr)
	{
		if (playCamGameObjId == id)
		{
			playCam = nullptr;
		}
	}

}

C_Camera* WG_Playbar::GetPlayCam()
{
	return playCam;
}

void WG_Playbar::RefreshPlayCam()
{
	GameObject* myGo = App->scene->GetGameObject(playCamGameObjId);

	if (myGo != nullptr)
	{
		C_Camera* newCam = myGo->GetComponent<C_Camera>();

		if (newCam != nullptr)
		{
			playCam = newCam;
			playCamGameObjId = myGo->GetUid();
			return;
		}

	}

	playCam = nullptr;
	playCamGameObjId = 0;

}

void WG_Playbar::ShowTimeMultiplier()
{
	float currTimeMultiplier = App->GetTimeMultiplier();

	ImGui::SameLine();
	ImGui::Text(" | Time Mult.");
	ImGui::SameLine();
	ImGui::PushItemWidth(120);
	if (ImGui::InputFloat("##timeMultiplier", &currTimeMultiplier, 0.1, 0.2, "%.2f"))
	{
		App->SetTimeMultiplier(currTimeMultiplier);
	}

}
