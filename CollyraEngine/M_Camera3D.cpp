#include "Globals.h"
#include "Application.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "M_Input.h"

M_Camera3D::M_Camera3D(MODULE_TYPE type, bool start_enabled) : Module(type, start_enabled), spdMultiplier(2.0f), editorCamera(true)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

}

M_Camera3D::~M_Camera3D()
{}

// -----------------------------------------------------------------
bool M_Camera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool M_Camera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
updateStatus M_Camera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	M_Input* inputModule = (M_Input*)App->GetModulePointer(M_INPUT);

	if (inputModule == nullptr)
	{
		return UPDATE_STOP;
	}

	if (inputModule->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		editorCamera = !editorCamera;

	if (editorCamera)
	{
		//Debug Cam

		vec3 newPos(0, 0, 0);
		float speed = 10.0f * dt;
		if (inputModule->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = (speed * 2);

		if (inputModule->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
		if (inputModule->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

		if (inputModule->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (inputModule->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (inputModule->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)newPos -= X * speed;
		if (inputModule->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		Position += newPos;
		Reference += newPos;

		// Mouse motion ----------------

		if (inputModule->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -inputModule->GetMouseXMotion();
			int dy = -inputModule->GetMouseYMotion();

			float Sensitivity = 0.25f;

			Position -= Reference;

			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			}

			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				Y = rotate(Y, DeltaY, X);
				Z = rotate(Z, DeltaY, X);

				if (Y.y < 0.0f)
				{
					Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = cross(Z, X);
				}
			}

			Position = Reference + Z * length(Position);
		}

	}


	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void M_Camera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void M_Camera3D::LookAt(const vec3& Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void M_Camera3D::Move(const vec3& Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* M_Camera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void M_Camera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

// -----------------------------------------------------------------
vec3 M_Camera3D::GetCameraPosition()
{
	return Position;
}
