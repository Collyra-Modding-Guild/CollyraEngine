#ifndef __WG_CONSOLE_H__
#define __WG_CONSOLE_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

#define FRAMERATE_LOG_SIZE 100

struct AppConsole
{

};

class WG_Console : public WindowGroup
{
public:
	WG_Console(bool isActive);
	virtual ~WG_Console();

	virtual updateStatus Update();

	virtual void Cleanup();

	void NewLog(const char* newInputLog);

private:
	// Portable helpers
	int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	char* Strdup(const char* s)
	{
		size_t len = strlen(s) + 1;
		void* buf = malloc(len);
		IM_ASSERT(buf);
		return (char*)memcpy(buf, (const void*)s, len);
	}
	void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void    ClearLog();
	void    AddLog(const char* fmt, ...);
	void    Draw(const char* title, bool* p_open);
	void    ExecCommand(const char* command_line);
	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
	int     TextEditCallback(ImGuiInputTextCallbackData* data);


private:

	AppConsole console;

	//Console Vars---
	char                  InputBuf[256];
	std::vector<char*>       Items;
	std::vector<const char*> Commands;
	std::vector<char*>       History;
	int                   HistoryPos;
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;

};

#endif // __WG_CONFIG_H__