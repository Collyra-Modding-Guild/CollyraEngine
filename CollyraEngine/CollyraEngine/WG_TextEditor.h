#ifndef __WG_TEXTEDITOR_H__
#define __WG_TEXTEDITOR_H__

#include "WindowGroup.h"
#include "Globals.h"
#include "Imgui/imgui.h"
#include "ImGuiTextEditor/TextEditor.h"
#include <string>
#include <map>
#include "p2Defs.h"

class WG_TextEditor : public WindowGroup
{
public:
	WG_TextEditor(bool isActive);
	virtual ~WG_TextEditor();

	virtual updateStatus Update();

	virtual void Cleanup();

private:
	void SetFile(const char* newFile, uint64 modTime);

private:
	TextEditor textEditor;
	std::string currFile;
	std::string currFileName;
	uint64		fileModTime;
};

#endif // __WG_TEXTEDITOR_H__