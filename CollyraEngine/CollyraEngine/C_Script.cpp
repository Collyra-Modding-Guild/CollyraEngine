#include "C_Script.h"
#include "GameObject.h"

C_Script::C_Script(bool active) : Component(COMPONENT_TYPE::SCRIPT, active), currentScriptClass("")
{


}

C_Script::~C_Script()
{ }

std::string C_Script::GetScriptClass()
{
    return currentScriptClass;
}

void C_Script::SetScriptClass(const char* newClass)
{
    currentScriptClass = newClass;
}
