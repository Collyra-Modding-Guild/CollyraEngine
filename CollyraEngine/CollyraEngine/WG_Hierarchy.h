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
	void HandleNodeInput(GameObject* currentGo);
	void HandleDragAndDrop(GameObject* currentGo);

	void DrpGameObject(GameObject* from, GameObject* to);
	void HandlePopUp();
	void PopUpHierarchy();
	void PopUpSceneName();


private:
	ImGuiTreeNodeFlags	flag;
	int					selected;
	int					popUpSelected;
	bool				changeSceneNamePopUp;
	char				sceneName[256];

	GameObject*			rootPointer;

};

#endif // __WG_HIERARCHY_H__
