#ifndef __WINDOWGROUP_H__
#define __WINDOWGROUP_H__

#include <vector>
#include "Globals.h"

enum WG_TYPE
{
	WG_NONE = -1,

	WG_CONFIG,
	WG_CONSOLE
};


class WindowGroup
{
public:
	WindowGroup(WG_TYPE id, bool isActive);
	virtual ~WindowGroup();

	void SetActive(bool setActive);
	bool GetActive();

	virtual updateStatus Update();

	virtual void CleanUp();

public:
	bool active;

protected:
	WG_TYPE id;



};

#endif // __WINDOWGROUP_H__