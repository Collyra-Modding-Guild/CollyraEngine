#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "PhysVehicle3D.h"
#include "glmath.h"


struct PhysVehicle3D;

#define MAX_ACCELERATION 2500.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define SMOOTH_ACROBATIC_SPEED 3.0f
#define CAP_ACROBATIC_SPEED 6.5f

#define BRAKE_POWER 300.0f
#define JUMP_FORCE 4000.0f
#define IMPULSE_FORCE 2000.0f
#define MAX_TORQUE 2000.0f
#define STICK_FORCE 10.0f

#define INITIAL_TURBO 33.0f

#define PLAYERS 2


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true, int playerNum = 0);
	virtual ~ModulePlayer();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool Draw();
	bool Reset();
	bool CleanUp();

	btVector3 WorldToLocal(float x, float y, float z);
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void PlayerInputs();

public:

	VehicleInfo car;
	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	float turbo;

	bool jumpImpulse;
	bool secondJump;
	bool canDrift;

	bool fieldContact;

	int playerNum;
	int goalNum;

	vec3 initialPos;
	vec3 groundRayCast;


private:


	// INPUTS FOR EACH PLAYER
	SDL_Scancode Forward[PLAYERS];
	SDL_Scancode Backward[PLAYERS];
	SDL_Scancode Right[PLAYERS];
	SDL_Scancode Left[PLAYERS];
	SDL_Scancode Jump[PLAYERS];
	SDL_Scancode Turbo[PLAYERS];
	SDL_Scancode Brake[PLAYERS];
	SDL_Scancode SwapCamera[PLAYERS];

	
};