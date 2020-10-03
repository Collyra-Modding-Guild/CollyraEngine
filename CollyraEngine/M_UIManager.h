#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>


class M_UIManager : public Module
{
public:

	M_UIManager(Application* app, bool start_enabled = true);
	~M_UIManager();

	bool Awake();
	bool Start();

	updateStatus PreUpdate(float dt);
	updateStatus PostUpdate(float dt);

	bool CleanUp();

private:
	updateStatus stopAppButton();

protected:
	bool showDemoWindow;

};

#endif // __ModuleAudio_H__
