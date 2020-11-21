#ifndef __WG_PLAYBAR_H__
#define __WG_PLAYBAR_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

class WG_Playbar : public WindowGroup
{
public:

	WG_Playbar(bool isActive);
	virtual ~WG_Playbar();

	virtual updateStatus Update();

	virtual void Cleanup();

private:
	void ShowTimeMultiplier();

};

#endif // __WG_PLAYBAR_H__
