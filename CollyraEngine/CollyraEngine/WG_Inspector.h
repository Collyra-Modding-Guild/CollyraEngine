#ifndef __WG_INSPECTOR_H__
#define __WG_INSPECTOR_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"
#include <string>

#define TAGS_NUM 3
#define LAYERS_NUM 3

class GameObject;
class C_Mesh;
class C_Material;
class C_Camera;
class C_Script;

class WG_Inspector : public WindowGroup
{
public:

	WG_Inspector(bool isActive);
	virtual ~WG_Inspector();

	virtual updateStatus Update();

	virtual void Cleanup();

	void		SetGameObject(int focusedGameObject);
	void		SetGameObject(GameObject* focusedGameObject);

	int			GetFocusedGameObjectId() const;
	GameObject* GetGameObject() const;

	void		OnDestroyedId(uint destroyedGameObject);
	bool		DeleteGameObject(int key);

	void		Enable();
	void		Disable();

private:
	void		DrawHeaderGameObject();
	void		DrawTransformComponent(ImGuiTreeNodeFlags_ flag);
	void		DrawMeshComponent(ImGuiTreeNodeFlags_ flag, C_Mesh* mesh);
	void		DrawMaterialComponent(ImGuiTreeNodeFlags_ flag, C_Material* material);
	void		DrawCameraComponent(ImGuiTreeNodeFlags_ flag, C_Camera* camera);
	bool		DrawScriptComponent(ImGuiTreeNodeFlags_ flag, C_Script* script, int index);
	void		AddComponentButtonDraw();

	std::string	GetNameFromComponentEnum(int num);
	void		HandlePopUp();

private:
	int			focusedId;
	GameObject* focusedGameObject;

	const char* tags[TAGS_NUM] = { "Default", "Player", " ... " };
	const char* currentTag;

	const char* layers[LAYERS_NUM] = { "Default", "Water", " ... " };
	const char* currentLayer;

	int			selectedNewComponentType;
	bool		craeteScriptPopUp;

};

#endif // __WG_INSPECTOR_H__