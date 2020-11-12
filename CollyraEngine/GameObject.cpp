#include "GameObject.h"
#include "Component.h"
#include "p2Defs.h"

#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"

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
	C_Transform* transform = this->GetComponent<C_Transform>();
	if (transform->hasUpdated == true)
	{
		transform->hasUpdated = false;
		transform->GenerateGlobalTransformationFrom(this->parent->GetComponent<C_Transform>()->GetLocalTransform());;
		
		for (int i = 0; i < children.size(); i++)
		{
			C_Transform* transform = children[i]->GetComponent<C_Transform>();
			if (transform != nullptr)
			{
				transform->hasUpdated = true;
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
	default:
		break;
	}

	c->SetGameObject(this);
	components.push_back(c);

	return c;
}