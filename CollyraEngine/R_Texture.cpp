#include "R_Texture.h"


R_Texture::R_Texture(unsigned int uid) : Resource(uid, R_TYPE::TEXTURE), textureId(0)
{}


R_Texture::~R_Texture()
{}

uint R_Texture::GetTextureId()
{
	return textureId;
}
