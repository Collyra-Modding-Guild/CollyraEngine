#include "WG_TextEditor.h"

WG_TextEditor::WG_TextEditor(bool isActive) : WindowGroup(WG_TEXT_EDITOR,isActive)
{
}

WG_TextEditor::~WG_TextEditor()
{
}

updateStatus WG_TextEditor::Update()
{
	return UPDATE_CONTINUE;
}

void WG_TextEditor::Cleanup()
{
}
