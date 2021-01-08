#ifndef __Resource_Script_H__
#define __Resource_Script_H__

#include "R_Resource.h"
#include <string>

typedef unsigned __int64 uint64;

class R_Script : public Resource
{
public:
	R_Script(unsigned int uid);
	virtual ~R_Script();

	const char* GetScriptClassName() const;
	void SetScriptClassName(const char* newClassName);

	const char* GetScriptHPath() const;
	void SetScriptHPath(const char* newClassName);
	const char* GetScriptCppPath() const;
	void SetScriptCppPath(const char* newClassName);

	uint64 GetScriptHFileModDate() const;
	void SetScriptHFileModDate(uint64 newTime);
	uint64 GetScriptCppFileModDate() const;
	void SetScriptCppFileModDate(uint64 newTime);

	const char* GetScriptHCode() const;
	void SetScriptHCode(const char* newClassName);
	const char* GetScriptCppCode() const;
	void SetScriptCppCode(const char* newClassName);

private:
	std::string className;
	std::string hFile, cppFile;
	uint64 hFileModDate, cppFileModDate;
	std::string hCode, cppCode;

};

#endif //__Resource_Script_H_
