#ifndef __WG_ASSETS_H__
#define __WG_ASSETS_H__

#include "WindowGroup.h"
#include "Imgui/imgui.h"
#include <string>
#include "Globals.h"

class WG_Assets : public WindowGroup
{
public:
	WG_Assets(bool isActive);
	virtual ~WG_Assets();

	virtual updateStatus Update();

	virtual void Cleanup();

private:
	void LoadNewAsset(std::string& toLoad);
	void LoadNewResource(std::string& toLoad);
	void DeleteAsset(std::string& toDelete);
	void DeleteResource(std::string& toDelete);

};

#endif // __WG_ASSETS_H__