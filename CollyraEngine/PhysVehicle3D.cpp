#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{	
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
	myBody = body;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render(int carPlayer)
{
	Cylinder wheel;

	if (carPlayer == 1)
		wheel.color.Set(50.f / 255.f, 255.f / 255.f, 224.f / 255.f);
	else
		wheel.color.Set(255.f / 255.f, 118.f / 255.f, 50.f / 255.f);

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[i].radius;
		wheel.height = info.wheels[i].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);
		
		
		wheel.Render();
	}



		

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();


	if (carPlayer == 1)
		chassis.color.Set(50.f / 255.f, 112.f / 255.f, 255.f / 255.f);
	else
		chassis.color.Set(255.f / 255.f, 77.f / 255.f, 77.f / 255.f);

	chassis.Render();

	for (int i = 0; i < info.num_chassis; ++i)
	{

		Cube chassis(info.chassis[i].chassis_size.x, info.chassis[i].chassis_size.y, info.chassis[i].chassis_size.z);
		vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
		btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
		btVector3 offset(info.chassis[i].chassis_offset.x, info.chassis[i].chassis_offset.y, info.chassis[i].chassis_offset.z);
		offset = offset.rotate(q.getAxis(), q.getAngle());


		if (carPlayer == 1)
			chassis.color.Set(114.f / 255.f, 202.f / 255.f, 246.f / 255.f);
		else
			chassis.color.Set(255.f / 255.f, 165.f / 255.f, 75.f / 255.f);
		

		chassis.transform.M[12] += offset.getX();
		chassis.transform.M[13] += offset.getY();
		chassis.transform.M[14] += offset.getZ();

	
		chassis.Render();

	}



}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ResetSpeed() 
{
	this->SetAngularVelocity({ 0,0,0 });
	this->SetLinealVelocity({ 0,0,0 });
}

