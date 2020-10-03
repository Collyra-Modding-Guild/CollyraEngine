#pragma once

class Application;

#include <string>



class Module
{
private:
	bool enabled;

public:
	Application* App;

	Module(Application* parent, bool start_enabled = true) : App(parent)
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

public:

	std::string				name;

};