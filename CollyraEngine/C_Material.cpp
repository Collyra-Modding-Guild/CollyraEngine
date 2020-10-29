#include "C_Material.h"
#include "TextureLoader.h"

#include "OpenGL.h"

C_Material::C_Material(bool active) : Component(COMPONENT_TYPE::MATERIAL, active), idTextureImage(0), color(Grey)
{}

C_Material::~C_Material()
{
	ClearTexture();
}

void C_Material::SetTexture(uint idTexture, bool clearPreviousTexture)
{
	if (clearPreviousTexture)
		ClearTexture();

	idTextureImage = idTexture;
}

void C_Material::SetTextureNameAndPath(const char* name, const char* path)
{
	texName = name;
	texPath = path;
}

void C_Material::SetColor(Color color)
{
	this->color = color;
}

void C_Material::SetDefaultTexture()
{
	ClearTexture();
	idTextureImage = TextureLoader::LoadDefaultTexture();

	SetTextureNameAndPath("Checkers", "Default");
}

uint C_Material::GetTexture() const
{
	return idTextureImage;
}

Color C_Material::GetColor() const
{
	return color;
}

void C_Material::ClearTexture()
{
	glDeleteTextures(1, &idTextureImage);
}
