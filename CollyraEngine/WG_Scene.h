#ifndef __WG_SCENE_H__
#define __WG_SCENE_H__

#include "WindowGroup.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"


class WG_Scene : public WindowGroup
{
public:
	WG_Scene(bool isActive);
	virtual ~WG_Scene();

	virtual updateStatus Update();

	virtual void Cleanup();

	void OnResize(ImVec2 newWindowSize);

	void GetWindowSize(float& w, float& h);
	void GetWindowPosition(float& x, float& y);

	void HandleGuizmo();


public:
	bool usingGizmo;

private:
	ImVec2 windowPos;
	ImVec2 windowSize;
	ImVec2 imageSize;

	ImGuizmo::OPERATION gizOperation;
	ImGuizmo::MODE gizMode;
};

#endif // __WG_SCENE_H__