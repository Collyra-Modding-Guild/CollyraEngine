#ifndef __WG_ASSETS_H__
#define __WG_ASSETS_H__

#include "WindowGroup.h"
#include "Globals.h"

class WG_TextEditor : public WindowGroup
{
public:
	WG_TextEditor(bool isActive);
	virtual ~WG_TextEditor();

	virtual updateStatus Update();

	virtual void Cleanup();

};

#endif // __WG_ASSETS_H__