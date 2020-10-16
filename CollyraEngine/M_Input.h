#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Vec2;

class M_Input : public Module
{
public:

	M_Input(MODULE_TYPE type, bool start_enabled = true);
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
		return mouseX;
	}

	int GetMouseY() const
	{
		return mouseY;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouseMotionX;
	}

	int GetMouseYMotion() const
	{
		return mouseMotionY;
	}

private:
	KEY_STATE* keyboard;
	KEY_STATE mouseButton[MAX_MOUSE_BUTTONS];

	int mouseX, mouseY;
	int mouseMotionX, mouseMotionY;
	int mouse_z;

};