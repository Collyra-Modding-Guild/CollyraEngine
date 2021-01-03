#pragma once

#include "Component.h"
#include "Globals.h"

class R_Script;
class CollObject;

class C_Script : public Component
{
public:

	C_Script(bool active = true);
	~C_Script();

	void SetResourceId(uint newId);
	int GetResourceId() const;

	void Update(float dt) override;

	std::string GetScriptClass();
	void		SetScriptClass(const char* newClass);

private:
	uint		scriptId;
	R_Script*	script;
	CollObject* dataObject;

};