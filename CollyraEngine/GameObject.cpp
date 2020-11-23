#include "GameObject.h"
#include "Component.h"
#include "p2Defs.h"

#include "Application.h"
#include "M_Scene.h"

#include "C_Transform.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Camera.h"

GameObject::GameObject(std::string name) : parent(nullptr), uid(0), name(name), active(true),
selected(false)
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
		if (components[i]->GetType() == COMPONENT_TYPE::CAMERA)
			App->scene->DeleteCamera(components[i]);

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
	C_Camera* camera = GetComponent<C_Camera>();

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

	if (camera != nullptr)
		camera->UpdateFrustum(transform->GetGlobalTransform());
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
		App->scene->cameras.push_back((C_Camera*)newComponent);
	}
	break;
	default:
		return nullptr;
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

const std::vector<Component*>* GameObject::GetAllComponents() const
{
	return &components;
}

uint32 GameObject::GetUid() const
{
	return uid;
}

void GameObject::SetUid(uint32 newId)
{
	this->uid = newId;
}

bool GameObject::IsSelected() const
{
	return selected;
}

void GameObject::SetSelected(bool newId)
{
	if (newId != selected)
		selected = !selected;
}

std::string GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(std::string newName)
{
	name = newName;
}

void GameObject::SetActive(bool newState)
{
	if (newState != active)
		active = !active;
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

	if (prevParent != parent)
	{
		parent->AddChildren(this);

		if (prevParent != nullptr)
		{
			prevParent->NotifyChildDeath(this);
			GetComponent<C_Transform>()->UpdateLocalTransformMaintingGlobalToFit(parent->GetComponent<C_Transform>()->GetGlobalTransform());
		}

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
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i] == newChild)
		{
			return;
		}
	}

	children.push_back(newChild);
}

GameObject* GameObject::GetChild(int id) const
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i]->GetUid() == id)
		{
			return children[i];
		}
	}
	return nullptr;
}

std::vector<GameObject*> GameObject::GetChilds() const
{
	return children;
}

const AABB GameObject::GetGameObjectAABB() const
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
		obb = AABB({ -0.5, -0.5, -0.5 }, { 0.5, 0.5, 0.5 });
		obb.Transform(GetComponent<C_Transform>()->GetGlobalTransform());

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
	case COMPONENT_TYPE::CAMERA: {}
							   break;
	default:
		break;
	}

	c->SetGameObject(this);
	components.push_back(c);

	return c;
}