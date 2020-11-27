#include "R_Material.h"
#include "R_Texture.h"
#include "M_Resources.h"
#include "Application.h"


R_Material::R_Material(unsigned int uid) : Resource(uid, R_TYPE::MATERIAL), idTextureResource(-1), myTexResource(nullptr)
{}


R_Material::~R_Material()
{
	myTexResource = nullptr;
	App->resources->UnloadResource(idTextureResource);
}

void R_Material::SetColor(Color color)
{
	this->color = color;
}

void R_Material::SetTextureResourceId(uint rTexId, bool load)
{
	App->resources->UnloadResource(idTextureResource);
	idTextureResource = rTexId;

	if(load)
	myTexResource = (R_Texture*)App->resources->RequestResource(idTextureResource);
}

Color R_Material::GetColor() const
{
	return color;
}

int R_Material::GetTextureResourceId() const
{
	return idTextureResource;
}

int R_Material::GetTextureId() const
{
	return myTexResource ? myTexResource->GetTextureId() : 0;
}

std::string R_Material::GetTextureName() const
{
	return myTexResource ? myTexResource->GetName() : "No Texture Loaded";
}

std::string R_Material::GetTexturePath() const
{
	return myTexResource ? myTexResource->GetLibraryPath() : "No Texture Loaded";
}


