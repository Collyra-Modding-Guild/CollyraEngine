#ifndef __ModuleUIManager_H__
#define __ModuleUIManager_H__

#include "Module.h"

#include <string>
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "Imgui/imgui.h"


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


	// Helpers-----
	void			NewInputLog(uint keyID, uint keyState, bool isMouse = false);
private:
	//Functions for the editor menus-----------
	updateStatus	stopAppButton();
	bool 			ShowMenuRandomTest();
	bool 			ShowMenuSphereCollisionTest();
	bool 			ShowMenuAABBCollisions();
	bool 			ShowMenuOBBCollisions();
	bool 			ShowMenuPlaneCollisions();
	bool 			ShowMenuRayCollisions();
	bool 			ShowMenuTriangleCollisions();
	void			ShowMainMenuBar();

	void			CalculateHistogramLogs();

protected:
	//Bool controllers for the editor menus-----------
	bool showDemoWindow;

	bool menuMathRandomTest;
		bool	generateRandomNumbers;
		int		generatedInt;
		float	generatedFloat;
		int		randomStartThreshold;
		int		randomEndThreshold;

		std::vector<float> fps_log;
		std::vector<float> ms_log;

		ImGuiTextBuffer logInputs;

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
