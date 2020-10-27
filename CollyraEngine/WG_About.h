#ifndef __WG_ABOUT_H__
#define __WG_ABOUT_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

#define ENGINE_GITHUB_URL "https://github.com/Collyra-Modding-Guild/CollyraEngine"

class WG_About : public WindowGroup
{
public:

	WG_About(bool isActive);
	virtual ~WG_About();

	virtual updateStatus Update();

	virtual void Cleanup();

private:

};

#endif // __WG_ABOUT_H__
