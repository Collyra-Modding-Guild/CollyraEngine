#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>


class ModuleUIManager : public Module
{
public:

	ModuleUIManager(Application* app, bool start_enabled = true);
	~ModuleUIManager();

	bool Init();

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

};

#endif // __ModuleAudio_H__
