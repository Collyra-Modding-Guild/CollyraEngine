#pragma once

#include "Component.h"
#include "Globals.h"

class R_Script;
class CollObject;

struct __declspec(dllexport) ReflectableScriptData {

	ReflectableScriptData(const std::string name, const std::string type, void* ptr, int varSize) 
	{
		this->name = name;
		this->type = type;
		this->ptr = ptr;
		this->varSize = varSize;
	}

	std::string name;
	std::string type;
	void* ptr = nullptr;
	int varSize;
};

enum class SERIALIZABLE_TYPE
{
	NO_TYPE = -1,
	BOOL,
	INT,
	FLOAT

};


struct __declspec(dllexport) SerializableScriptData {

	SerializableScriptData(const std::string name, const SERIALIZABLE_TYPE type, void* ptr, int varSize)
	{
		this->name = name;
		this->type = type;
		this->ptr = ptr;
		this->varSize = varSize;
	}

	std::string name;
	SERIALIZABLE_TYPE type;
	void* ptr = nullptr;
	int varSize;
};


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

	void DeleteObjectData(bool deleteReflectableVars = true);
	void GenerateObjectData();
	CollObject* GetObjectData() const;

	bool AddReflectVariable(std::string name, std::string type, void* ptr, int size);
	bool AddSerializeVariable(std::string name, std::string type, void* ptr, int size);
	void SaveReflectableVariables();
	void LoadReflectableVariables();

	void SaveSerilizableVariables();
	void LoadSerilizableVariables();

public:
	std::vector<SerializableScriptData> prevSerializedVariables;
	std::vector<SerializableScriptData> serializedVariables;

private:
	uint		scriptId;
	R_Script*	myScript;
	CollObject* dataObject;

	std::vector<ReflectableScriptData> prevReflectableVariables;
	std::vector<ReflectableScriptData> reflectableVariables;


};