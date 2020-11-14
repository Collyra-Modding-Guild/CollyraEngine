#include "GameObject.h"
#include "Component.h"
#include "p2Defs.h"

#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Camera.h"

GameObject::GameObject(std::string name) : parent(nullptr), id(0), name(name), active(true)
{}

GameObject::~GameObject()
{
	for (int i = 0; i < children.size(); i++)
	{
		RELEASE(children[i]);
	}

	children.clear();
	parent = nullptr;

	for (int i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}
	components.clear();
}

void GameObject::Start()
{
	if (!active)
		return;

	for (uint i = 0; i < components.size(); i++)
	{
		components[i]->Start();
	}
}

void GameObject::Update(float dt)
{
	if (!active)
		return;

	for (uint i = 0; i < components.size(); i++)
	{
		if (components[i]->IsActive() == true)
			components[i]->Update(dt);
	}
}

void GameObject::PostUpdate(float dt)
{
	C_Transform* transform = GetComponent<C_Transform>();
	if (transform->hasUpdated == true)
	{
		transform->hasUpdated = false;
		transform->GenerateGlobalTransformationFrom(parent->GetComponent<C_Transform>()->GetGlobalTransform());;
		
		for (int i = 0; i < children.size(); i++)
		{
			C_Transform* childTransform = children[i]->GetComponent<C_Transform>();
			if (childTransform != nullptr)
			{
				childTransform->hasUpdated = true;
			}
		}
	}

	BoundingBoxUpdate();
}

Component* GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* newComponent = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		newComponent = new C_Transform();
	}
	break;
	case COMPONENT_TYPE::MESH:
	{
		newComponent = new C_Mesh();
	}
	break;
	case COMPONENT_TYPE::MATERIAL:
	{
		newComponent = new C_Material();
	}
	break;
	case COMPONENT_TYPE::CAMERA:
	{
		newComponent = new C_Camera();
	}
	break;
	default:
		break;
	}

	//We add the component to the GameObject
	// If the GameObject has it already (or a number of them), we don't add the component & we delete the created one
	// Only affects Transform for now :)
	if (newComponent != nullptr)
	{
		Component* ret = AddComponent(newComponent);

		if (ret != newComponent)
		{
			delete newComponent;
			newComponent = nullptr;
			return ret;
		}
	}

	newComponent->Start();

	return newComponent;
}

unsigned int GameObject::GetId() const
{
	return id;
}

void GameObject::SetId(unsigned int newId)
{
	this->id = newId;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(std::string newName)
{
	name = newName;
}

void GameObject::SetParent(GameObject* newParent)
{
	if (newParent == nullptr)
	{
		LOG("An GameObject Parent Cannot be NULL");
		return;
	}

	GameObject* prevParent = parent;
	parent = newParent;

	if (prevParent != parent && prevParent != nullptr)
	{
		GetComponent<C_Transform>()->UpdateLocalTransformMaintingGlobalToFit(parent->GetComponent<C_Transform>()->GetGlobalTransform());
	}
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::NotifyChildDeath(GameObject* deadChild)
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i] == deadChild)
		{
			children.erase(children.begin() + i);
		}
	}
}


void GameObject::AddChildren(GameObject* newChild)
{
	children.push_back(newChild);
}

GameObject* GameObject::GetChild(int id) const
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i]->GetId() == id)
		{
			return children[i];
		}
	}
	return nullptr;
}

const AABB GameObject::GetGameObjectAABB()
{
	return aabb;
}

void GameObject::BoundingBoxUpdate()
{
	C_Mesh* check = GetComponent<C_Mesh>();

	if (check) 
	{
		// Generate global OBB
		obb = GetComponent<C_Mesh>()->GetAABB();
		obb.Transform(GetComponent<C_Transform>()->GetGlobalTransform());

		// Generate global AABB
		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);
	}
	else
	{
		obb.Transform(GetComponent<C_Transform>()->GetGlobalTransform());
		
		aabb.SetNegativeInfinity();
		aabb.SetFromCenterAndSize(obb.CenterPoint(), float3(1, 1, 1));
		obb = aabb;
	}
}

Component* GameObject::AddComponent(Component* c)
{
	if (c == nullptr)
		return c;

	//Components limits check
	switch (c->GetType())
	{
	case COMPONENT_TYPE::TRANSFORM:
	{
		C_Transform* hasTransform = this->GetComponent<C_Transform>();
		if (hasTransform != nullptr)
		{
			LOG("GameObject already has a Transform Component!!!")
				return hasTransform;
		}
	}
	break;
	case COMPONENT_TYPE::MESH: {}
							 break;
	case COMPONENT_TYPE::MATERIAL: {}
							 break;
	case COMPONENT_TYPE::CAMERA: {}
							 break;
	default:
		break;
	}

	c->SetGameObject(this);
	components.push_back(c);

	return c;
}