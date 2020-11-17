#include "JsonConfig.h"

JsonConfig::JsonConfig() : rootValue(nullptr), rootObject(nullptr)
{
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);
}

JsonConfig::JsonConfig(JSON_Object* jObj) : rootValue(nullptr), rootObject(jObj)
{}

JsonConfig::~JsonConfig()
{


	if (rootValue)
	{
		json_value_free(rootValue);
	}
}

uint JsonConfig::Serialize(char** buffer)
{
	uint fileSize = json_serialization_size_pretty(rootValue);
	*buffer = new char[fileSize];
	json_serialize_to_buffer_pretty(rootValue, *buffer, fileSize);
	return fileSize;
}

JsonArray JsonConfig::CreateElementArray(const char* name)
{
	json_object_set_value(rootObject, name, json_value_init_array());
	return JsonArray(json_object_get_array(rootObject, name));
}

void JsonConfig::ReleaseFile()
{
	if (rootObject)
	{
		json_object_clear(rootObject);
	}
}

void JsonConfig::AddNumber(const char* tag, double value)
{
	json_object_set_number(rootObject, tag, value);
}

void JsonConfig::AddBool(const char* tag, bool value)
{
	json_object_set_boolean(rootObject, tag, value);
}

void JsonConfig::AddString(const char* tag, const char* value)
{
	json_object_set_string(rootObject, tag, value);
}


// Json Array------------------------------------------
JsonArray::JsonArray() : elementArray(nullptr), size(0)
{
	elementArray = json_value_get_array(json_value_init_array());
}

JsonArray::JsonArray(JSON_Array* loadedArray) : elementArray(loadedArray), size(0)
{
	size = json_array_get_count(elementArray);
}

JsonArray::~JsonArray()
{
}

JsonConfig JsonArray::AddValue()
{
	json_array_append_value(elementArray, json_value_init_object());
	size++;
	return JsonConfig(json_array_get_object(elementArray, size - 1));
}

void JsonArray::AddNumber(double value)
{
	json_array_append_number(elementArray, value);
	size++;
}

void JsonArray::AddBool(bool value)
{
	json_array_append_boolean(elementArray, value);
	size++;
}

void JsonArray::AddString(const char* value)
{
	json_array_append_string(elementArray, value);
	size++;
}

void JsonArray::AddFloat3(float3 value)
{
	json_array_append_number(elementArray, value.x);
	json_array_append_number(elementArray, value.y);
	json_array_append_number(elementArray, value.z);
	size += 3;
}

void JsonArray::AddQuat(Quat value)
{
	json_array_append_number(elementArray, value.x);
	json_array_append_number(elementArray, value.y);
	json_array_append_number(elementArray, value.z);
	json_array_append_number(elementArray, value.w);
	size += 4;
}
