#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"


class M_UIManager : public Module
{
public:

	M_UIManager(Application* app, bool start_enabled = true);
	~M_UIManager();

	bool Awake();
	bool Start();

	updateStatus PreUpdate(float dt);
	updateStatus Update(float dt);

	updateStatus PostUpdate(float dt);

	bool CleanUp();

private:
	//Functions for the editor menus-----------
	updateStatus	stopAppButton();
	bool 			ShowMenuRandomTest();
	void			ShowMainMenuBar();

protected:
	//Bool controllers for the editor menus-----------
	bool showDemoWindow;

	bool menuMathRandomTest;
		bool	generateRandomNumbers;
		int		generatedInt;
		float	generatedFloat;

	LCG randomSeed;

};

#endif // __ModuleAudio_H__
