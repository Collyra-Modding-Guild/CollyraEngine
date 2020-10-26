#include "WG_Inspector.h"
#include "Application.h"


WG_Inspector::WG_Inspector(bool isActive) : WindowGroup(WG_INSPECTOR, isActive)
{

}

WG_Inspector::~WG_Inspector()
{

}

updateStatus WG_Inspector::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Inspector");

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Inspector::Cleanup()
{}