#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class M_Input : public Module
{
public:
	
	M_Input(Application* app, bool start_enabled = true);
	~M_Input();

	bool Awake();
	updateStatus PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouseButton[id];
	}

	int GetMouseX() const
	{
		return mouse.x;
	}

	int GetMouseY() const
	{
		return mouse.y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouseMotion.x;
	}

	int GetMouseYMotion() const
	{
		return mouseMotion.y;
	}

private:
	KEY_STATE* keyboard;
	KEY_STATE mouseButton[MAX_MOUSE_BUTTONS];

	iMPoint mouse;
	iMPoint mouseMotion;
	int mouse_z;
};