#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "Imgui/imgui.h"

class WindowGroup;
class WG_Config;
class WG_Console;


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

	updateStatus Draw2D(float dt);

	// Helpers-----
	void			NewInputLog(uint keyID, uint keyState, bool isMouse = false);
	void			NewConsoleLog(const char* newLog);
	void			NewFpsLog(float currMs, float currFps);
	bool			IsDebugModeOn();


private:
	//Functions for the editor menus-----------


	bool			ShowMainMenuBar();

	bool 			ShowMenuRandomTest();
	bool 			ShowMenuSphereCollisionTest();
	bool 			ShowMenuAABBCollisions();
	bool 			ShowMenuOBBCollisions();
	bool 			ShowMenuPlaneCollisions();
	bool 			ShowMenuRayCollisions();
	bool 			ShowMenuTriangleCollisions();


public:
	std::vector<WindowGroup*> windowGroups;

	WG_Config* configWindow;
	WG_Console* consoleWindow;

protected:
	//Bool controllers for the editor menus-----------
	bool showDemoWindow;

	bool showConfigMenu;

	bool menuMathRandomTest;
	
		bool	generateRandomNumbers;
		int		generatedInt;
		float	generatedFloat;
		int		randomStartThreshold;
		int		randomEndThreshold;



		bool	sphereCollisionTest;
		math::Sphere sph1;
		math::Sphere sph2;

		bool	AABBCollisionTest;
		math::AABB AABB1;
		math::AABB AABB2;

		bool	OBBCollisionTest;
		math::OBB OBB1;
		math::OBB OBB2;

		bool	planeCollisionTest;
		math::Plane plane1;
		math::Plane plane2;

		bool	rayCollisionTest;
		math::Ray ray1;
		math::Ray ray2;

		bool	triangleCollisionTest;
		math::Triangle tri1;
		math::Triangle tri2;
	
	LCG randomSeed;


};

#endif // __ModuleAudio_H__
