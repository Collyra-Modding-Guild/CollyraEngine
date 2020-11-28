#ifndef __WG_ASSETS_H__
#define __WG_ASSETS_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"
#include "Globals.h"

class WG_Assets : public WindowGroup
{
public:
	WG_Assets(bool isActive);
	virtual ~WG_Assets();

	virtual updateStatus Update();

	virtual void Cleanup();

};

#endif // __WG_ASSETS_H__