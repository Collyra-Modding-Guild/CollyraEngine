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
	bool 			ShowMenuSphereCollisionTest();
	bool 			ShowMenuAABBCollisions();
	void			ShowMainMenuBar();

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

		bool	sphereCollisionTest;
		math::Sphere sph1;
		math::Sphere sph2;

		bool	AABBCollisionTest;
		math::AABB boundingBox1;
		math::AABB boundingBox2;



	LCG randomSeed;

};

#endif // __ModuleAudio_H__
