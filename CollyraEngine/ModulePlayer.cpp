#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled, int playerNum) : Module(app, start_enabled), vehicle(NULL), playerNum(playerNum)
{
	turn = acceleration = brake = 0.0f;
	groundRayCast = { 0,0,0 };
	goalNum = 0;

	// INPUTS FOR EACH PLAYER
	Forward[0] = {SDL_SCANCODE_W };
	Forward[1] = { SDL_SCANCODE_UP };

	Backward[0] = { SDL_SCANCODE_S};
	Backward[1] = { SDL_SCANCODE_DOWN };

	Right[0] = { SDL_SCANCODE_D };
	Right[1] = { SDL_SCANCODE_RIGHT };

	Left[0] = { SDL_SCANCODE_A };
	Left[1] = { SDL_SCANCODE_LEFT };

	Jump[0] = { SDL_SCANCODE_SPACE };
	Jump[1] = { SDL_SCANCODE_KP_0 };

	Turbo[0] = { SDL_SCANCODE_LSHIFT };
	Turbo[1] = { SDL_SCANCODE_RSHIFT };

	Brake[0] = { SDL_SCANCODE_B };
	Brake[1] = { SDL_SCANCODE_KP_1 };

	SwapCamera[0] = { SDL_SCANCODE_R };
	SwapCamera[1] = { SDL_SCANCODE_KP_5 };

	switch (playerNum)
	{
	case 1:
		initialPos = { 0, 6, -160 };
		break;

	case 2:
		initialPos = { 0, 6, 160 };
		break;

	}


}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");


	// Car properties ----------------------------------------

	//All chassis parts
	car.num_chassis = 5;
	car.chassis = new Chassis[car.num_chassis];

	//front mudward
	car.chassis[0].chassis_size.Set(2, 0.5, 1);
	car.chassis[0].chassis_offset.Set(0, 1.f, 2.5f);

	//back mudward
	car.chassis[1].chassis_size.Set(2, 0.5, 1);
	car.chassis[1].chassis_offset.Set(0, 1.f, -2.5f);

	//spoiler
	car.chassis[2].chassis_size.Set(0.1f, 0.6f, 0.2f);
	car.chassis[2].chassis_offset.Set(-0.5, 1.6f, -2.75f);

	car.chassis[3].chassis_size.Set(0.1f, 0.6f, 0.2f);
	car.chassis[3].chassis_offset.Set(0.5, 1.6f, -2.75f);

	car.chassis[4].chassis_size.Set(2.f, 0.2f, 0.4f);
	car.chassis[4].chassis_offset.Set(0.f, 2.f, -2.75f);


	car.chassis_size.Set(2, 1, 4);
	car.chassis_offset.Set(0, 1, 0);
	car.mass = 540.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 1.0f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 0.8f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x * 0.5f;
	float half_length = car.chassis_size.z * 0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[car.num_wheels];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.4f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	jumpImpulse = false;
	canDrift = false;
	secondJump = false;

	turbo = INITIAL_TURBO;
	
	

	vehicle = App->physics->AddVehicle(car);
	vehicle->collision_listeners.add(this);
	vehicle->cntType = CNT_VEHICLE;
	vehicle->SetPos(initialPos.x, initialPos.y, initialPos.z);
	if (playerNum == 2)
	{
		mat4x4 trans;
		vehicle->GetTransform(&trans);
		trans.rotate(180, {0, -1, 0});
		vehicle->SetTransform(&trans);
	}


	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

update_status ModulePlayer::PreUpdate(float dt)
{
	if(App->scene_intro->state != MT_STOP && App->scene_intro->state != MT_RESTARTING)
	PlayerInputs();

	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	groundRayCast = App->physics->RayCast({ this->vehicle->GetPos().x,this->vehicle->GetPos().y+1, this->vehicle->GetPos().z }, vehicle->GetDown());
	if (length(groundRayCast) < 2.f )
	{
		fieldContact = true;
		secondJump = false;
		jumpImpulse = false;
		vehicle->Push(0.0f, -STICK_FORCE/4, 0.0f);
	}
	else
  		fieldContact = false;

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

bool ModulePlayer::Draw()
{
	vehicle->Render(playerNum);
	return true;
}

// World to Local forces translation
btVector3 ModulePlayer::WorldToLocal(float x, float y, float z) 
{
	btVector3 relativeForce = btVector3(x, y, z);
	btMatrix3x3& localRot = vehicle->myBody->getWorldTransform().getBasis();
	btVector3 correctedForce = localRot * relativeForce;

	return correctedForce;
}


void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1->cntType == CNT_VEHICLE && body2->cntType == CNT_LITTLE_BOOST)
	{
		if (body2->sensorOnline)
		{
			turbo += 12.0f;
			App->audio->PlayFx(App->scene_intro->boostUpFx);
		}
	}

	if (body1->cntType == CNT_VEHICLE && body2->cntType == CNT_BIG_BOOST)
	{
		if (body2->sensorOnline)
		{
			turbo += 100.0f;
			App->audio->PlayFx(App->scene_intro->boostUpFx);
		}
	}

	if (turbo > 100.0f)
		turbo = 100.0f;
}

bool ModulePlayer::Reset()
{

	mat4x4 mat;
	btTransform identity;
	identity.setIdentity();
	identity.getOpenGLMatrix(&mat);

	switch (playerNum)
	{
	case 1:
		break;

	case 2:
		mat.rotate(180, { 0, -1, 0 });
		break;
	}

	vehicle->SetTransform(&mat);

	vehicle->ResetSpeed();

	vehicle->SetPos(initialPos.x, initialPos.y, initialPos.z);

	this->turbo = 33.f;

	return true;
}

void ModulePlayer::PlayerInputs()
{
	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(Forward[playerNum - 1]) == KEY_REPEAT && fieldContact && vehicle->GetKmh() < 180)
	{

		if(vehicle->GetKmh() <= 0)
			acceleration = MAX_ACCELERATION * 5;
		else
			acceleration = MAX_ACCELERATION;

		vehicle->Push(0.0f, -STICK_FORCE, 0.0f);
	}
	else if (App->input->GetKey(Forward[playerNum - 1]) == KEY_REPEAT && !fieldContact)
	{
		if (vehicle->myBody->getAngularVelocity().length() < CAP_ACROBATIC_SPEED)
		{
			if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
				vehicle->myBody->applyTorque(WorldToLocal(5000.0f, 0.0f, 0.0f));
			else
				vehicle->myBody->applyTorque(WorldToLocal(500.0f, 0.0f, 0.0f));
		}
	}

	if (App->input->GetKey(Left[playerNum - 1]) == KEY_REPEAT && fieldContact)
	{
		if (turn < TURN_DEGREES && !canDrift)
			turn += TURN_DEGREES;
		else if (turn > -TURN_DEGREES && canDrift)
		{
			turn += TURN_DEGREES;

			vehicle->myBody->applyTorque(WorldToLocal(0.0f, 20000.0f, 0.0f));
		}

		vehicle->Push(0.0f, -STICK_FORCE, 0.0f);

	}
	else if (App->input->GetKey(Left[playerNum - 1]) == KEY_REPEAT && !fieldContact)
	{
		if (vehicle->myBody->getAngularVelocity().length() < CAP_ACROBATIC_SPEED)
		{
			if (secondJump)
			{
				if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, -5000.0f));
				else
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, -500.0f));


			}
			else
			{
				if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 5000.0f, 0.0f));
				else
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 500.0f, 0.0f));
			}
		}

	}



	if (App->input->GetKey(Right[playerNum - 1]) == KEY_REPEAT && fieldContact)
	{
		if (turn > -TURN_DEGREES && !canDrift)
			turn -= TURN_DEGREES;
		else if (turn > -TURN_DEGREES && canDrift)
		{
			turn -= TURN_DEGREES;

			vehicle->myBody->applyTorque(WorldToLocal(0.0f, -20000.0f, 0.0f));
			
		}

		vehicle->Push(0.0f, -STICK_FORCE, 0.0f);

	}
	else if (App->input->GetKey(Right[playerNum - 1]) == KEY_REPEAT && !fieldContact)
	{

		if (vehicle->myBody->getAngularVelocity().length() < CAP_ACROBATIC_SPEED)
		{
			if (secondJump)
			{
				if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, 5000.0f));
				else
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, 500.0f));
			}
			else
			{
				if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, -5000.0f, 0.0f));
				else
					vehicle->myBody->applyTorque(WorldToLocal(0.0f, -500.0f, 0.0f));
			}
		}

	}

	if (App->input->GetKey(Backward[playerNum - 1]) == KEY_REPEAT && fieldContact && vehicle->GetKmh() > -120)
	{

		if (vehicle->GetKmh() >= 0)
			acceleration = -MAX_ACCELERATION * 5;
		else
			acceleration = -MAX_ACCELERATION;

		vehicle->Push(0.0f, -STICK_FORCE, 0.0f);
	}
	else if (App->input->GetKey(Backward[playerNum - 1]) == KEY_REPEAT && !fieldContact)
	{
		if (vehicle->myBody->getAngularVelocity().length() < CAP_ACROBATIC_SPEED)
		{
			if (vehicle->myBody->getAngularVelocity().length() < SMOOTH_ACROBATIC_SPEED)
				vehicle->myBody->applyTorque(WorldToLocal(-5000.0f, 0.0f, 0.0f));
			else
				vehicle->myBody->applyTorque(WorldToLocal(-500.0f, 0.0f, 0.0f));
		}
	}

	if (App->input->GetKey(Jump[playerNum - 1]) == KEY_DOWN && fieldContact)
	{
		vehicle->myBody->setAngularVelocity({ 0,0,0 });
		vehicle->Push(0.0f, JUMP_FORCE, 0.0f);
		fieldContact = false;

	}
	else if (App->input->GetKey(Jump[playerNum - 1]) == KEY_DOWN && !fieldContact && !secondJump)
	{
		secondJump = true;

		vehicle->Push(0.0f, IMPULSE_FORCE, 0.0f);


		if (App->input->GetKey(Forward[playerNum - 1]) == KEY_REPEAT)
		{
			jumpImpulse = true;
			vehicle->myBody->applyCentralForce(WorldToLocal(0.0f, 0.0f, 300000.0f));
			vehicle->myBody->applyTorque(WorldToLocal(115000.0f, 0.0f, 0.0f));
		}
		else if (App->input->GetKey(Backward[playerNum - 1]) == KEY_REPEAT)
		{
			jumpImpulse = true;
			vehicle->myBody->applyCentralForce(WorldToLocal(0.0f, 0.0f, -300000.0f));
			vehicle->myBody->applyTorque(WorldToLocal(-115000.0f, 0.0f, 0.0f));
		}
		else if (App->input->GetKey(Right[playerNum - 1]) == KEY_REPEAT)
		{
			jumpImpulse = true;
			vehicle->myBody->applyCentralForce(WorldToLocal(-400000.0f, 0.0f, 0.0f));
			vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, 45000.0f));
		}
		else if (App->input->GetKey(Left[playerNum - 1]) == KEY_REPEAT)
		{
			jumpImpulse = true;
			vehicle->myBody->applyCentralForce(WorldToLocal(400000.0f, 0.0f, 0.0f));
			vehicle->myBody->applyTorque(WorldToLocal(0.0f, 0.0f, -45000.0f));
		}


	}

	if (App->input->GetKey(Brake[playerNum - 1]) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
		canDrift = true;

	}
	else if (App->input->GetKey(Brake[playerNum - 1]) == KEY_UP)
		canDrift = false;


	if (App->input->GetKey(Turbo[playerNum - 1]) == KEY_REPEAT && turbo > 0)
	{
		vehicle->myBody->applyCentralImpulse(WorldToLocal(0.0f, 0.0f, 125.0f));
		turbo -= 0.5f;
	}

	if (App->input->GetKey(SwapCamera[playerNum - 1]) == KEY_DOWN)
	{
		if (playerNum == 1)
			App->camera->lookAtBall = !App->camera->lookAtBall;
		else
			App->camera_2->lookAtBall = !App->camera_2->lookAtBall;

	}


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

}