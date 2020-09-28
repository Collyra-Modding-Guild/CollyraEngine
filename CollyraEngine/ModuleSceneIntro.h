#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Bullet/include/LinearMath/btScalar.h"
#include "PhysVehicle3D.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

enum matchState
{
	MT_NONE,

	MT_STOP,
	MT_RUNNING,
	MT_GOAL,
	MT_RESTARTING
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true, uint matchMinutes = 5u);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	update_status PreUpdate (float dt);
	bool CleanUp();
	bool Draw();
	bool Reset();
	void ResetBall();
	void RestartMatch();
	void ApplyGoalForce(PhysVehicle3D* scoringPlayer, PhysVehicle3D* playerThatWasScored);
	void LoadMapAssets();
	void DebugInputs();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	//----------PRIMITIVES---------------//
	p2DynArray<Primitive*> primitives;

	//---------------VEHICLE--------------//
	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	vec3 ballInitialPos;

	Primitive* matchBall = nullptr;

	Timer matchtimer;
	Timer matchStoppedTimer;
	matchState state;

	bool readyToRestart;
	bool ballContactGround;

	int countDownFx;
	int goalFx;
	int matchFinishFx;
	int boostUpFx;

	uint matchTime;

	//SDL_Texture* fontTexture;
};
