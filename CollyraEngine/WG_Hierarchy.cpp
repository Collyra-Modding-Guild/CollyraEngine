#include "WG_Hierarchy.h"
#include "Application.h"

WG_Hierarchy::WG_Hierarchy(bool isActive) : WindowGroup(WG_HIERARCHY, isActive)
{

}

WG_Hierarchy::~WG_Hierarchy()
{

}

updateStatus WG_Hierarchy::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("Hierarchy");


	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_Hierarchy::Cleanup()
{}