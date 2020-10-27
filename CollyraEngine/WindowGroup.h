#ifndef __WINDOWGROUP_H__
#define __WINDOWGROUP_H__

#include <vector>
#include "Globals.h"

enum WG_TYPE
{
	WG_NONE = -1,

	WG_CONFIG,
	WG_CONSOLE,
	WG_INSPECTOR,
	WG_HIERARCHY,
	WG_ABOUT

};

class ImVec2;

class WindowGroup
{
public:
	WindowGroup(WG_TYPE id, bool isActive);
	virtual ~WindowGroup();

	void SetActive(bool setActive);
	bool GetActive();

	virtual updateStatus Update();
	virtual void CleanUp();

	virtual void GetWindowSize(int& w,int& h);

public:
	bool active;

protected:
	WG_TYPE id;
};

#endif // __WINDOWGROUP_H__