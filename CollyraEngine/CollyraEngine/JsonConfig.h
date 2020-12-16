#ifndef __JsonConfig_H__
#define __JsonConfig_H__

#include "parson\parson.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class JsonArray;

class JsonConfig
{
public:
	JsonConfig();
	JsonConfig(JSON_Object* jObj);
	JsonConfig(const char* buffer);
	~JsonConfig();

	uint Serialize(char** buffer);
	JsonArray CreateElementArray(const char* name);

	//When loading, and creating different files, must be created for the first when your are done
	void ReleaseFile();

	//Atributtes Add-------
	void AddNumber(const char* tag, double value);
	void AddBool(const char* tag, bool value);
	void AddString(const char* tag, const char* value);

	//Atributtes Get-------
	double GetNumber(const char* tag);
	bool GetBool(const char* tag);
	std::string GetString(const char* tag);
	JsonArray GetArray(const char* name);

	bool IsInitialized();

private:
	JSON_Value* rootValue;
	JSON_Object* rootObject;

};

class JsonArray
{
public:
	JsonArray();
	JsonArray(JSON_Array* loadedArray);
	~JsonArray();

	JsonConfig AddValue();

	bool IsInitialized();

	//Atributtes Add-------
	void AddNumber(double value);
	void AddBool(bool value);
	void AddString(const char* value);

	void AddFloat3(float3 value);
	void AddQuat(Quat value);

	//Atributtes get-------------
	float3 GetFloat3(uint index = 0);
	Quat GetQuaternion(uint index = 0);
	double GetNumber(uint index);
	uint Size() const;
	JsonConfig GetNode(uint index) const;

private:
	JSON_Array* elementArray;
	uint size;
};

#endif //__SceneLoader_H__