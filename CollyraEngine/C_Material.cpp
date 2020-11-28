#include "C_Material.h"

#include "Application.h"
#include "M_Resources.h"
#include "R_Material.h"

#include "OpenGL.h"

C_Material::C_Material(bool active) : Component(COMPONENT_TYPE::MATERIAL, active), useDefaultTexture(false),
defaultTextureId(App->resources->defaultTextureId), resourceId(0), myResource(nullptr)
{}

C_Material::~C_Material()
{
	App->resources->UnloadResource(resourceId);
}

void C_Material::SetUseDefaultTexture(bool defaultTextureOn)
{
	if (defaultTextureOn != useDefaultTexture)
		useDefaultTexture = !useDefaultTexture;
}

bool C_Material::GetDefaultTextureUsage() const
{
	return useDefaultTexture;
}

uint C_Material::GetTexture() const
{
	if (useDefaultTexture == true)
		return defaultTextureId;

	return myResource ? myResource->GetTextureId() : 0;
}

Color C_Material::GetColor() const
{
	return myResource ? myResource->GetColor() : Grey;
}

std::string C_Material::GetMaterialName() const
{
	if (useDefaultTexture == true)
		return "Checkers";

	return myResource ? myResource->GetName() : "No Material Loaded";
}

std::string C_Material::GetMaterialPath() const
{
	if (useDefaultTexture == true)
		return "Default";

	return myResource ? myResource->GetLibraryPath() : "No Material Loaded";
}

std::string C_Material::GetTextureName() const
{
	return myResource ? myResource->GetTextureName() : "No Texture Loaded";
}

std::string C_Material::GetTexturePath() const
{
	return myResource ? myResource->GetTexturePath() : "No Texture Loaded";
}

void C_Material::SetResourceId(uint newId)
{
	App->resources->UnloadResource(resourceId);

	resourceId = newId;
	myResource = (R_Material*)App->resources->RequestResource(resourceId);
}

int C_Material::GetResourceId() const
{
	return resourceId;
}

void C_Material::ResourceUpdated(std::vector<unsigned int>* ids)
{
	for (uint i = 0; i < ids->size(); i++)
	{
		if (ids->at(i) == resourceId)
		{
			SetResourceId(ids->at(i));
		}
		else if (myResource && ids->at(i) == myResource->GetTextureResourceId())
		{
			myResource->SetTextureResourceId(ids->at(i));
		}

	}
}
