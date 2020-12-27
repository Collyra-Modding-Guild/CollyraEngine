#pragma once

class Application;

#include "Globals.h"

enum MODULE_TYPE
{
	NO_TYPE = -1,

	M_WINDOW,
	M_RENDER3D,
	M_INPUT,
	M_CAMERA3D,
	M_UIMANAGER,
	M_FILEMANAGER,
	M_RESOURCES,
	M_SCENE,
	M_SCRIPTINTERFACE
};


class Module
{
private:
	bool enabled;

public:

	Module(MODULE_TYPE type, bool startEnabled = true) :type(type), enabled(startEnabled)
	{}

	virtual ~Module()
	{}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual updateStatus PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual updateStatus Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual updateStatus PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	bool IsEnabled() const
	{
		return enabled;
	}

	void Enable()
	{
		if (enabled == false)
		{
			enabled = true;
			Start();
		}
	}

	void Disable()
	{

		if (enabled == true)
		{
			enabled = false;
			CleanUp();
		}

	}

	virtual bool Reset() { return true; }

	virtual updateStatus Draw2D(float dt) { return UPDATE_CONTINUE; }
	virtual updateStatus Draw(bool* drawState) { return UPDATE_CONTINUE; }
	virtual updateStatus PreDraw(bool* drawState) { return UPDATE_CONTINUE; }

	MODULE_TYPE GetType() { return type; }

public:

	MODULE_TYPE type;

};