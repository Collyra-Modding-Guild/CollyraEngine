#ifndef __WG_CONFIG_H__
#define __WG_CONFIG_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"
#include "Globals.h"

#define FRAMERATE_LOG_SIZE 100

class M_Input;
class M_Window;

class WG_Config : public WindowGroup
{
public:
	WG_Config(bool isActive);
	virtual ~WG_Config();

	virtual updateStatus Update();

	virtual void Cleanup();

	void NewLogInput(const char* newInputLog);

	void NewLogFramerate(float newMs, float newFps);

	bool* GetDrawFlags();

private:
	std::vector<float> fpsLog;
	std::vector<float> msLog;

	ImGuiTextBuffer logInputs;
	bool			newInput;

	bool			drawFlags[MAX_FLAGS];

};

#endif // __WG_CONFIG_H__