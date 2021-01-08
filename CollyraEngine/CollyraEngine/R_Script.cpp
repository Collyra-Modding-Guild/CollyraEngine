#include "R_Script.h"
#include "M_Resources.h"
#include "Application.h"

R_Script::R_Script(unsigned int uid) : Resource(uid, R_TYPE::SCRIPT),
className(""), hFile(""), cppFile(""), hCode(""), cppCode(""), hFileModDate(0), cppFileModDate(0)
{}

R_Script::~R_Script()
{
}

const char* R_Script::GetScriptClassName() const
{
	return className.c_str();
}

void R_Script::SetScriptClassName(const char* newClassName)
{
	className = newClassName;
}

const char* R_Script::GetScriptHPath() const
{
	return hFile.c_str();
}

void R_Script::SetScriptHPath(const char* newClassName)
{
	hFile = newClassName;
}

const char* R_Script::GetScriptCppPath() const
{
	return cppFile.c_str();
}

void R_Script::SetScriptCppPath(const char* newClassName)
{
	cppFile = newClassName;
}

uint64 R_Script::GetScriptHFileModDate() const
{
	return hFileModDate;
}

void R_Script::SetScriptHFileModDate(uint64 newTime)
{
	hFileModDate = newTime;
}

uint64 R_Script::GetScriptCppFileModDate() const
{
	return cppFileModDate;
}

void R_Script::SetScriptCppFileModDate(uint64 newTime)
{
	cppFileModDate = newTime;
}

const char* R_Script::GetScriptHCode() const
{
	return hCode.c_str();
}

void R_Script::SetScriptHCode(const char* newClassName)
{
	hCode = newClassName;
}

const char* R_Script::GetScriptCppCode() const
{
	return cppCode.c_str();
}

void R_Script::SetScriptCppCode(const char* newClassName)
{
	cppCode = newClassName;
}
