#include "Application.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "M_Input.h"

#include "MathGeoLib/include/Geometry/Frustum.h"

M_Camera3D::M_Camera3D(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), spdMultiplier(2.0f), editorCamera(true)
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
	bool ret = true;

	LOG("Setting up the camera");
	inputModule = (M_Input*)App->GetModulePointer(M_INPUT);

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

	if (inputModule == nullptr)
	{
		return UPDATE_STOP;
	}

	CameraMovement(dt);

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void M_Camera3D::CameraMovement(float dt)
{
	vec3 newPos(0, 0, 0);
	float speed = DEFAULT_MOUSE_SPEED * dt;

	if (inputModule->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = (speed * 2);

	if (inputModule->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if (inputModule->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	vec3 zoom = CameraZoom(dt);

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

		if (inputModule->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (inputModule->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (inputModule->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)newPos -= X * speed;
		if (inputModule->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;	

		Position = Reference + Z * length(Position);
	}

	/*ProvisionalReference();
	float2 focusSpace = Frustum::ScreenToViewportSpace({ focus.x, focus.y }, App->window->screenWidth, App->window->screenHeight);
	focus = (focusSpace.x, focusSpace.y, focus.z);

	Look(Position, focus, true);*/
	

	Position += newPos + zoom;
	Reference += newPos + zoom;

}

// -----------------------------------------------------------------
vec3 M_Camera3D::CameraZoom(float dt)
{
	vec3 zoom(0, 0, 0);

	int wheelInput = App->input->GetMouseZ();

	if (wheelInput > 0)
		zoom -= Z * DEFAULT_WHEEL_SPEED * dt;
	else if (wheelInput < 0)
		zoom += Z * DEFAULT_WHEEL_SPEED * dt;

	return zoom;
}

// -----------------------------------------------------------------
void M_Camera3D::CameraFocus() 
{

}

// -----------------------------------------------------------------
void M_Camera3D::CameraOrbital() 
{

}

// -----------------------------------------------------------------
void M_Camera3D::ProvisionalReference()
{
	int X = App->input->GetMouseX();
	int Y = App->input->GetMouseY();
	int Z = 500;

	if (App->input->GetMouseButton(1) == KEY_DOWN) 
	{
		focus = (X, Y, Z);
		LOG("X: %d  Y: %d  Z: %d", X, Y, Z);
	}		
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
