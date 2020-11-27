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

	void NewFocusedGameObject(int id);
	void OnDestroyedId(int id);

private:
	void CreateHierarchy(GameObject* parent);
	void SetTreeNodeFlags(GameObject* parent);
	void HandleDragAndDrop(GameObject* currentGo);

	void DrpGameObject(GameObject* from, GameObject* to);


private:
	ImGuiTreeNodeFlags	flag;
	int					selected;

	GameObject*	rootPointer;

};

#endif // __WG_HIERARCHY_H__
