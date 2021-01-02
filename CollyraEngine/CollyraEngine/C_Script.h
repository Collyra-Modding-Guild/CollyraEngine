#pragma once

#include "Component.h"
#include "Globals.h"


class C_Script : public Component
{
public:

	C_Script(bool active = true);
	~C_Script();

	std::string GetScriptClass();
	void		SetScriptClass(const char* newClass);


private:
	std::string currentScriptClass;

};