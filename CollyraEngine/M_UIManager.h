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

	bool Init();
	bool Start();

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

private:
	update_status stopAppButton();

protected:
	bool showDemoWindow;
	bool showDemoWindow2;

};

#endif // __ModuleAudio_H__
