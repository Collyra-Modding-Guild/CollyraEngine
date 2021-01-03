#include "ScriptLoader.h"
#include "JsonConfig.h"
#include "Application.h"
#include "M_FileManager.h"
#include "R_Script.h"

std::string ScriptLoader::Create(const char* className, const char* cppPath, const char* hPath, const char* cppCode, const char* hCode)
{
	char* buffer = nullptr;
	JsonConfig jsonFile;
	jsonFile.AddString("Class", className);


	jsonFile.AddString("CppPath", cppPath);
	jsonFile.AddNumber("CppFileModTime", App->physFS->GetCurrDate(cppPath));

	jsonFile.AddString("HPath", hPath);
	jsonFile.AddNumber("HFileModTime", App->physFS->GetCurrDate(hPath));


	jsonFile.AddString("CppCode", cppCode);
	jsonFile.AddString("hCode", hCode);

	uint size = jsonFile.Serialize(&buffer);
	jsonFile.ReleaseFile();

	std::string saveTo = std::string(ASSETS_SCRIPTS_PATH).append("/").append(className).append(".").append(SCRIPT_EXTENSION);
	App->physFS->Save(saveTo.c_str(), buffer, size);
	RELEASE(buffer);

	return saveTo;
}

void ScriptLoader::Import(const char* scriptAssetPath, char** buffer, R_Script* myNewScript)
{
	JsonConfig jsonFile(*buffer);
	if (jsonFile.IsInitialized())
	{

		myNewScript->SetScriptClassName(jsonFile.GetString("Class").c_str());

		myNewScript->SetScriptCppPath(jsonFile.GetString("CppPath").c_str());
		myNewScript->SetScriptCppFileModDate(jsonFile.GetNumber("CppFileModTime"));

		myNewScript->SetScriptHPath(jsonFile.GetString("HPath").c_str());
		myNewScript->SetScriptHFileModDate(jsonFile.GetNumber("HFileModTime"));


		myNewScript->SetScriptCppCode(jsonFile.GetString("CppCode").c_str());
		myNewScript->SetScriptHCode(jsonFile.GetString("hCode").c_str());

	}
}

uint ScriptLoader::Save(R_Script* myNewScript, char** buffer)
{
	JsonConfig jsonFile;
	jsonFile.AddString("Class", myNewScript->GetScriptClassName());


	jsonFile.AddString("CppPath", myNewScript->GetScriptCppPath());
	jsonFile.AddNumber("CppFileModTime", myNewScript->GetScriptCppFileModDate());

	jsonFile.AddString("HPath", myNewScript->GetScriptHPath());
	jsonFile.AddNumber("HFileModTime", myNewScript->GetScriptHFileModDate());


	jsonFile.AddString("CppCode", myNewScript->GetScriptCppCode());
	jsonFile.AddString("hCode", myNewScript->GetScriptHCode());

	uint size = jsonFile.Serialize(buffer);
	jsonFile.ReleaseFile();
	return size;
}

bool ScriptLoader::Load(R_Script* myNewScript, char* buffer)
{
	JsonConfig jsonFile(buffer);
	if (jsonFile.IsInitialized())
	{
		myNewScript->SetScriptClassName(jsonFile.GetString("Class").c_str());

		myNewScript->SetScriptCppPath(jsonFile.GetString("CppPath").c_str());
		myNewScript->SetScriptCppFileModDate(jsonFile.GetNumber("CppFileModTime"));

		myNewScript->SetScriptHPath(jsonFile.GetString("HPath").c_str());
		myNewScript->SetScriptHFileModDate(jsonFile.GetNumber("HFileModTime"));


		myNewScript->SetScriptCppCode(jsonFile.GetString("CppCode").c_str());
		myNewScript->SetScriptHCode(jsonFile.GetString("hCode").c_str());
	}

	return true;
}

