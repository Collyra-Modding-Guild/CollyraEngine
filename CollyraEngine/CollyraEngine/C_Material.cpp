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

void C_Material::SetColor(Color newColor) 
{
	return myResource ? myResource->SetColor(newColor) : 0;
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
	int prevId = resourceId;

	resourceId = newId;
	myResource = (R_Material*)App->resources->RequestResource(resourceId);

	App->resources->UnloadResource(prevId);
}

int C_Material::GetResourceId() const
{
	return resourceId;
}

R_Material* C_Material::GetResourcePointer() const
{
	return myResource;
}

void C_Material::ResourceUpdated(std::map<uint, bool>* ids)
{
	std::map<uint, bool>::iterator it = ids->find(resourceId);

	if (it != ids->end())
	{
		if (it->second == true)
			SetResourceId(it->first);
		else
		{
			App->resources->UnloadResource(resourceId);
			myResource = nullptr;
			resourceId = 0;
		}
	}

	if (myResource != nullptr)
	{
		it = ids->find(myResource->GetTextureResourceId());

		if (it != ids->end())
		{
			if (it->second == true)
			{
				myResource->SetTextureResourceId(it->first);
			}
			else
			{
				myResource->SetTextureResourceId(it->first, false);
			}
		}
	}


}
