#ifndef __WG_PLAYBAR_H__
#define __WG_PLAYBAR_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"

class C_Camera;

class WG_Playbar : public WindowGroup
{
public:

	WG_Playbar(bool isActive);
	virtual ~WG_Playbar();

	virtual updateStatus Update();

	virtual void Cleanup();

	void OnDestroyedId(unsigned int id);

	C_Camera*	 GetPlayCam();
	void		RefreshPlayCam();

private:
	void ShowTimeMultiplier();

private:

	C_Camera* playCam;
	unsigned int playCamGameObjId;

};

#endif // __WG_PLAYBAR_H__
