#include "JsonConfig.h"

JsonConfig::JsonConfig() : rootValue(nullptr), rootObject(nullptr)
{
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);
}

JsonConfig::JsonConfig(JSON_Object* jObj) : rootValue(nullptr), rootObject(jObj)
{}

JsonConfig::JsonConfig(const char* buffer)
{
	rootValue = json_parse_string(buffer);
	if (rootValue)
	{
		rootObject = json_value_get_object(rootValue);
	}
	else
	{
		LOG("Error creating Json file from buffer");
		rootObject = nullptr;
	}
}

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

double JsonConfig::GetNumber(const char* tag)
{
	if (json_object_has_value_of_type(rootObject, tag, JSONNumber))
	{
		return json_object_get_number(rootObject, tag);
	}

	return NULL;
}

bool JsonConfig::GetBool(const char* tag)
{
	if (json_object_has_value_of_type(rootObject, tag, JSONBoolean))
	{
		return json_object_get_boolean(rootObject, tag);
	}

	return NULL;
}

std::string JsonConfig::GetString(const char* tag)
{
	if (json_object_has_value_of_type(rootObject, tag, JSONString))
	{
		return json_object_get_string(rootObject, tag);
	}

	return "";
}

JsonArray JsonConfig::GetArray(const char* name)
{
	if (json_object_has_value_of_type(rootObject, name, JSONArray))
		return JsonArray(json_object_get_array(rootObject, name));
	else
	{
		LOG("Array with tag %s not found!", name);
		return JsonArray();
	}
}

bool JsonConfig::IsInitialized()
{
	return (rootObject);
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

bool JsonArray::IsInitialized()
{
	return (elementArray);
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

float3 JsonArray::GetFloat3(uint index)
{
	float3 ret = { 0,0,0 };
	ret.x = GetNumber(index);
	ret.y = GetNumber(index + 1);
	ret.z = GetNumber(index + 2);

	return ret;
}

Quat JsonArray::GetQuaternion(uint index)
{
	Quat ret = Quat::identity;
	ret.x = GetNumber(index);
	ret.y = GetNumber(index + 1);
	ret.z = GetNumber(index + 2);
	ret.w = GetNumber(index + 3);

	return ret;
}

double JsonArray::GetNumber(uint index)
{
	if (index < size)
	{
		return json_array_get_number(elementArray, index);
	}
	else
	{
		LOG("Error loading number from array at index %i", index);
		return NULL;
	}
}

uint JsonArray::Size() const
{
	return size;
}

JsonConfig JsonArray::GetNode(uint index) const
{
	return JsonConfig(json_array_get_object(elementArray, index));
}
