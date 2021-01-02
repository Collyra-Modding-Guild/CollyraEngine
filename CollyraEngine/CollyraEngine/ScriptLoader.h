#ifndef __ScriptLoader_H__
#define __ScriptLoader_H__

#include <string>
#include "Globals.h"

class R_Script;

namespace ScriptLoader
{
	std::string Create(const char* className, const char* cppPath, const char* hPath, const char* cppCode, const char* hCode);

	void Import(const char* scriptAssetPath, char** buffer, R_Script* myNewScript);
	uint Save(R_Script* myNewScript, char** buffer);
	//bool Load(char* buffer);

}


#endif //__ScriptLoader_H__