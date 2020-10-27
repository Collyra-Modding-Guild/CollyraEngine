#ifndef __WG_HIERARCHY_H__
#define __WG_HIERARCHY_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

class GameObject;

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

private:
	const GameObject* rootPointer;

};

#endif // __WG_HIERARCHY_H__
