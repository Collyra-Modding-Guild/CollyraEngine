#ifndef __WG_RESOURCE_COUNT_H__
#define __WG_RESOURCE_COUNT_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

enum class R_TYPE;

class WG_ResourceCount : public WindowGroup
{
public:

	WG_ResourceCount(bool isActive);
	virtual ~WG_ResourceCount();

	virtual updateStatus Update();

	virtual void Cleanup();

	void WG_ResourceCount::DrawResourceType(R_TYPE type);

};

#endif // __WG_RESOURCE_COUNT_H__
