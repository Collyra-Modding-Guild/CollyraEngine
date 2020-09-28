#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"
#include "ModuleWindow.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled, uint camNum) : Module(app, start_enabled), cameraNum(camNum)
{
	App->cam_list.add(this);

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	camViewPort = { 0,0,0,0 };

	//We calculate the "amount" of screen the window will ocupate
	switch (this->cameraNum) {

	case 1:
		camViewPort.x = 0;
		camViewPort.y = 0;
		camViewPort.w = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.h = App->window->SCREEN_HEIGHT;
		break;
	case 2:
		camViewPort.x = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.y = 0;
		camViewPort.w = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.h = App->window->SCREEN_HEIGHT;
		break;
	}
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	switch (this->cameraNum) {

	case 1:
		target_vehicle = App->player->vehicle;
		target_player = App->player;
		break;
	case 2:
		target_vehicle = App->player_2->vehicle;
		target_player = App->player_2;
		break;
	}

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		cameraDebug = !cameraDebug;

	
	if (cameraDebug)
	{
		//Debug Cam
		
		vec3 newPos(0,0,0);
		float speed = 10.0f * dt;
		if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = (speed *2) * dt;

		if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
		if(App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

		if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) newPos -= Z * speed;
		if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) newPos += Z * speed;


		if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) newPos -= X * speed;
		if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) newPos += X * speed;

		Position += newPos;
		Reference += newPos;

		// Mouse motion ----------------

		if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.25f;

			Position -= Reference;

			if(dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			}

			if(dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				Y = rotate(Y, DeltaY, X);
				Z = rotate(Z, DeltaY, X);

				if(Y.y < 0.0f)
				{
					Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = cross(Z, X);
				}
			}

			Position = Reference + Z * length(Position);
		}
		
	}
	else
		//REGULAR CAMERA
	{
		vec3 vehiclePos = target_vehicle->GetPos();
		camDistanceFromCar = { 0,3.5,-7.5f };

		if (lookAtBall)
		{
			//LOOKING AT THE BALL

			//The direction of the ball to the car
			vec3 dir = vehiclePos -App->scene_intro->matchBall->body->GetPos();

			//Check if the cam is too close to the ball
			if (length(dir) < 10)
			{
				if(multiplier < 1.5f)
				multiplier += 0.0125f;
			}
			else
			{
				if(multiplier > 1.0f)
				multiplier -= 0.025f;
			}

			////Normalize the vector & get only the x and z components
			dir = normalize(dir);
			dir.Set(dir.x,0.f,dir.z);

			//We calculate the new position: vehiclePos + the normalized direction augmented  +y offset
			newpos = vehiclePos + (dir * -camDistanceFromCar.z * multiplier) + vec3(0, camDistanceFromCar.y, 0);

			//Interpolate the new distance to have a smooth transition
			btVector3 interpolVec = lerp({ Position.x, Position.y, Position.z }, { newpos.x, newpos.y, newpos.z }, LERP_VALUE);
			Position = { interpolVec.x() , interpolVec.y() , interpolVec.z() };

			LookAt(App->scene_intro->matchBall->body->GetPos());
		}
		else
		{
			//We calculate the rotation & distance where the camera has to be set
			if (target_player->fieldContact)
			{
				mat4x4 transform;
				target_vehicle->GetTransform(&transform);
				mat3x3 rotationLocal(transform);

				newpos = vehiclePos + rotationLocal * camDistanceFromCar;

				rotation = rotationLocal;
			}
				//In case the player is not touching the ground, we only calculate the position
			else
			{
				newpos = vehiclePos + rotation * camDistanceFromCar;
			}

			//Change the camera position with lerp
			btVector3 interpolVec = lerp({Position.x, Position.y, Position.z}, { newpos.x, newpos.y, newpos.z }, LERP_VALUE);
			Position = { interpolVec.x() , interpolVec.y() , interpolVec.z() };

			LookAt(vehiclePos + vec3(0, camDistanceFromCar.y, 0));
		}
	}


	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

// -----------------------------------------------------------------
vec3 ModuleCamera3D::GetCameraPosition()
{
	return Position;
}

// -----------------------------------------------------------------
SDL_Rect ModuleCamera3D::getViewPort()
{
	return camViewPort;
}

// -----------------------------------------------------------------
//Change the camera viewport when the window size is alterated
void ModuleCamera3D::ReSizeViewPorts()
{
	switch (this->cameraNum) {

	case 1:
		camViewPort.x = 0;
		camViewPort.y = 0;
		camViewPort.w = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.h = App->window->SCREEN_HEIGHT;
		break;
	case 2:
		camViewPort.x = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.y = 0;
		camViewPort.w = App->window->SCREEN_WIDTH * 0.5f;
		camViewPort.h = App->window->SCREEN_HEIGHT;

		break;
	}


}