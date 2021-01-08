#pragma once

#include "Component.h"
#include "Globals.h"

class R_Script;
class CollObject;

class __declspec(dllexport) C_Script : public Component
{
public:

	C_Script(bool active = true);
	~C_Script();

	void SetResourceId(uint newId);
	int GetResourceId() const;

	void Start() override;
	void Update(float dt) override;

	void OnEnable() override;
	void OnDisable() override;

	std::string GetScriptClass();
	void		SetScriptClass(const char* newClass);

	void ResourceUpdated(std::map<uint, bool>* ids);

	void DeleteObjectData();
	void GenerateObjectData();
	CollObject* GetObjectData() const;

private:
	uint		scriptId;
	R_Script*	myScript;
	CollObject* dataObject;

};