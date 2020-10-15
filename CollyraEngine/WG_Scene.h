#ifndef __WG_SCENE_H__
#define __WG_SCENE_H__

#include "WindowGroup.h"

//ImGui
#include "imgui/imgui.h"

class WG_Scene : public WindowGroup
{
public:
	WG_Scene(bool isActive);
	virtual ~WG_Scene();

	virtual updateStatus Update();

	virtual void Cleanup();

	void OnResize(ImVec2 newWindowSize);

	void GetWindowSize(int& w, int& h);

private:
	ImVec2 windowSize;
	ImVec2 imageSize;

};

#endif // __WG_SCENE_H__