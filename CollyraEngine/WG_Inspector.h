#ifndef __WG_INSPECTOR_H__
#define __WG_INSPECTOR_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

#define TAGS_NUM 3
#define LAYERS_NUM 3

class GameObject;

class WG_Inspector : public WindowGroup
{
public:

	WG_Inspector(bool isActive);
	virtual ~WG_Inspector();

	virtual updateStatus Update();

	virtual void Cleanup();

	void SetGameObject(uint focusedGameObject);
	int GetFocusedGameObjectId() const;
	void SetGameObject(GameObject* focusedGameObject);
	GameObject* GetGameObject() const;
	void OnDestroyedId(uint destroyedGameObject);

	void DrawTransformComponent (ImGuiTreeNodeFlags_ flag);
	void DrawMeshComponent(ImGuiTreeNodeFlags_ flag);
	void DrawMaterialComponent(ImGuiTreeNodeFlags_ flag);

	void DrawHeaderGameObject();

private:
	int focusedId;
	GameObject* focusedGameObject;

	const char* tags[TAGS_NUM] = { "Default", "Player", " ... " };
	const char* currentTag;

	const char* layers[LAYERS_NUM] = { "Default", "Water", " ... " };
	const char* currentLayer;

};

#endif // __WG_INSPECTOR_H__