#ifndef __WG_HIERARCHY_H__
#define __WG_HIERARCHY_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

class GameObject;

#define ROOT 0

class WG_Hierarchy : public WindowGroup
{
public:

	WG_Hierarchy(bool isActive);
	virtual ~WG_Hierarchy();

	virtual updateStatus Start();
	virtual updateStatus Update();

	virtual void Cleanup();

private:
	void CreateHierarchy(const GameObject* parent);
	void SetTreeNodeFlags(const GameObject* parent);

private:
	ImGuiTreeNodeFlags	flag;
	int					selected;

	const GameObject*	rootPointer;

};

#endif // __WG_HIERARCHY_H__
