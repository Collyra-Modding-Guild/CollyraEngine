#include "Application.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Scene.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"

#include "MathGeoLib/include/Geometry/Frustum.h"

M_Camera3D::M_Camera3D(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), spdMultiplier(2.0f), focusingObject(true)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
	orbitalReference = vec3(0.0f, 0.0f, -15.0f);
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

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FocusGameObject(App->scene->focusedGameObject);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void M_Camera3D::CameraMovement(float dt)
{
	vec3 newPos(0, 0, 0);
	float speed = DEFAULT_MOUSE_SPEED * dt;

	//Acceleration-------
	if (inputModule->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = (speed * 2);

	CameraPlanePan(newPos);

	vec3 zoom = CameraZoom(dt);

	if (inputModule->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		OrbitArroundReference(Reference, newPos, speed);
		this->orbitalReference = this->Position;
		this->orbitalReference -= Z * 20;
	}

	else if(inputModule->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && inputModule->GetKey(SDL_SCANCODE_LALT))
	{
		OrbitArroundReference(orbitalReference, newPos, speed);
		this->Reference = this->Position;
		this->Reference -= Z * 5;
	}

	//If we are rotating arround a GameObject, we only change the focus when we move
	// This allows free zoom arround the focused GameObject
	if ((newPos.x > 0 || newPos.y > 0 || newPos.z > 0.f) && focusingObject == true)
	{
		ResetReference();
		focusingObject = false;
	}

	Move(newPos);
	Move(zoom, !focusingObject);
}

// -----------------------------------------------------------------
void M_Camera3D::CameraPlanePan(vec3& newPos)
{
	if (inputModule->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = inputModule->GetMouseXMotion();
		int dy = inputModule->GetMouseYMotion();

		float Sensitivity = 0.15f;

		newPos += Y * dy * Sensitivity;
		newPos -= X * dx * Sensitivity;
	}
}

void M_Camera3D::OrbitArroundReference(vec3& reference, vec3& pos, vec3 speed)
{
	int dx = -inputModule->GetMouseXMotion();
	int dy = -inputModule->GetMouseYMotion();

	float Sensitivity = 0.25f;

	Position -= reference;

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

	if (inputModule->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) pos -= Z * speed;
	if (inputModule->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) pos += Z * speed;


	if (inputModule->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)pos -= X * speed;
	if (inputModule->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) pos += X * speed;

	Position = reference + Z * length(Position);
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
void M_Camera3D::ResetReference()
{
	this->Reference = this->Position;
	this->Reference -= Z * 5;

	this->orbitalReference = this->Position;
	this->orbitalReference -= Z * 20;
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

	this->orbitalReference = this->Position;
	this->orbitalReference -= Z * 20;

	CalculateViewMatrix();
}

void M_Camera3D::OrbitalLook(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	this->orbitalReference = Reference;
	this->Position = orbitalReference + length(Position) * Z;

	if (!RotateAroundReference)
	{
		this->orbitalReference = this->Position;
		this->Position += Z * 0.05f;
	}
	else
	{
		focusingObject = true;
	}

	this->Reference = this->Position;
	this->Reference -= Z * 5;

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
void M_Camera3D::Move(const vec3& Movement, bool changeReference)
{
	Position += Movement;
	Reference += Movement;

	if (changeReference == true)
	{
		orbitalReference += Movement;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void M_Camera3D::FocusGameObject(GameObject* focused, float multiplier)
{
	if (focused != nullptr)
	{
		vec3 pos = { 0.0f, 0.0f, 0.0f };
		float dist = length({ 10.0f, 10.0f, 10.0f });

		C_Transform* transform = focused->GetComponent<C_Transform>();
		float3 transformPos = transform->GetGlobalTransform().Col3(3);
		pos = { transformPos.x, transformPos.y, transformPos.z };

		C_Mesh* mesh = focused->GetComponent<C_Mesh>();

		if (mesh != nullptr)
		{
			float3 meshSize = mesh->GetSize();
			dist = length({ meshSize.x, meshSize.y, meshSize.z });
			multiplier *= 0.75;
		}

		vec3 Direction = { 1.0f, 1.0f, 1.0f };
		vec3 unitDirection = normalize(Direction);

		vec3 newPos = { 1.0f, 1.0f, 1.0f };;
		if (dist > 0.0f)
			newPos = pos + unitDirection * dist;
		
		OrbitalLook(newPos * multiplier, pos, true);
	}
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

