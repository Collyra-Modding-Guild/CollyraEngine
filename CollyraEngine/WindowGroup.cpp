#include "WindowGroup.h"
#include "Globals.h"

#include "imgui/imgui.h"

WindowGroup::WindowGroup(WG_TYPE myId, bool isActive) : id(myId), active(isActive)
{}

WindowGroup::~WindowGroup()
{}

void WindowGroup::SetActive(bool setActive)
{
	active = setActive;
}

bool WindowGroup::GetActive()
{
	return active;
}

updateStatus WindowGroup::Update()
{
	return UPDATE_CONTINUE;
}

void WindowGroup::CleanUp()
{}

void WindowGroup::GetWindowSize(int& w, int& h)
{
	ImVec2 size = ImGui::GetWindowSize();

	w = size.x;
	h = size.y;
}
