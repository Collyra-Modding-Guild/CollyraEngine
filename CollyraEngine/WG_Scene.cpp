#include "WG_Scene.h"
#include "Application.h"
#include "M_Window.h"
#include "Globals.h"
#include "M_Renderer3D.h"

WG_Scene::WG_Scene(bool isActive) : WindowGroup(WG_CONFIG, isActive)
{}

WG_Scene::~WG_Scene()
{}

updateStatus WG_Scene::Update()
{
	updateStatus ret = UPDATE_CONTINUE;

	ImGui::Begin("Scene");
	{
		// Using a Child allow to fill all the space of the window.
		// It also alows customization
		ImGui::BeginChild("GameRender");

		ImVec2 winSize = ImGui::GetWindowSize();
		if (winSize.x != windowSize.x || winSize.y != windowSize.y)
			OnResize(winSize);

		// We Flip the UVs
		ImVec2 uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right

		ImGui::Image((ImTextureID)App->renderer3D->texColorBuffer, winSize, uv_min, uv_max);

		ImGui::EndChild();
	}

	ImGui::End();

	return ret;
}

void WG_Scene::Cleanup()
{}

void WG_Scene::OnResize(ImVec2 newWindowSize)
{
	windowSize.x = newWindowSize.x;
	windowSize.y = newWindowSize.y;

	App->renderer3D->OnResize(newWindowSize.x, newWindowSize.y);
}

void WG_Scene::GetWindowSize(float& w, float& h)
{
	w = windowSize.x;
	h = windowSize.y;
}

