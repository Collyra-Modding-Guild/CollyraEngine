#ifndef __WG_HIERARCHY_H__
#define __WG_HIERARCHY_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"


class WG_Hierarchy : public WindowGroup
{
public:

	WG_Hierarchy(bool isActive);
	virtual ~WG_Hierarchy();

	virtual updateStatus Update();

	virtual void Cleanup();

private:

};

#endif // __WG_HIERARCHY_H__
