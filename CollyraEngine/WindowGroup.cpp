#include "WindowGroup.h"
#include "Globals.h"

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
