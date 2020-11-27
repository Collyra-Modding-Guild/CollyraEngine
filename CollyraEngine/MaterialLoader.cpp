#include "MaterialLoader.h"
#include "TextureLoader.h"

#include "Application.h"
#include "M_FileManager.h"
#include "R_Material.h"
#include "M_Resources.h"

#include "Color.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

void MaterialLoader::Import(const aiMaterial* material, R_Material* myNewMaterial, const char* matName, const char* meshPath)
{
	//Color----
	aiColor4D materialColor;
	if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &materialColor) == aiReturn_SUCCESS)
	{
		myNewMaterial->SetColor(Color{ materialColor.r, materialColor.g, materialColor.b, materialColor.a });
	}
	myNewMaterial->SetNameAndPath(matName, "");

	//Texture---
	aiString path;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{
		std::string pathRelativeToFbx = path.C_Str();
		std::string relativePath = "";
		std::string textureName = "";
		App->physFS->SplitFilePath(meshPath, nullptr, &relativePath, &textureName, nullptr);
		pathRelativeToFbx = App->physFS->GetProjectPathFromInternalRelative(relativePath, pathRelativeToFbx);

		//IMPORT--------
		char* compressedTextureBuffer = nullptr;
		std::string texturePath = LIBRARY_TEXTURES;
		uint compressedTextureSize;
		uint loadTexture = App->resources->ImportResourceFromAssets(pathRelativeToFbx.c_str());

		if (loadTexture == 0)
		{
			LOG("Checking if texture exists in Textures Folder....");
			std::string textureName = "";
			std::string textureExtension = "";
			App->physFS->SplitFilePath(path.C_Str(), nullptr, nullptr, &textureName, &textureExtension);
			std::string defaultPath = ASSETS_TEXTURES_PATH;
			defaultPath += "/" + textureName + "." + textureExtension;
			loadTexture = App->resources->ImportResourceFromAssets(defaultPath.c_str());
		}

		//LOAD----
		if (loadTexture != 0)
		{
			LOG("Texture loaded! ID: %i", loadTexture);
			myNewMaterial->SetTextureResourceId(loadTexture);
		}
		else
		{
			LOG("Material's texture found but could not be loaded. Texture not found in path given by the importer: %s", pathRelativeToFbx.c_str());
		}
	}
}

uint MaterialLoader::Save(const R_Material* ourMaterial, char** buffer)
{
	//Color - Text Name - Text Path - Name - Path
	//TODO: Change material name + path -> UID

	uint ranges[4] = { 4, 1,
		ourMaterial->GetName().length(),ourMaterial->GetPath().length() };

	uint size = sizeof(ranges) + sizeof(float) * 4 +
		sizeof(int) + ourMaterial->GetName().length() + ourMaterial->GetPath().length();

	*buffer = new char[size]; // Allocate
	char* cursor = *buffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store Color
	bytes = sizeof(float) * 4;
	memcpy(cursor, &ourMaterial->GetColor(), bytes);
	cursor += bytes;

	// Store Text. Id
	bytes = sizeof(int);
	int textureId = ourMaterial->GetTextureId();
	memcpy(cursor, &textureId, bytes);
	cursor += bytes;

	// Store Mat. Name
	bytes = sizeof(char) * ourMaterial->GetName().length();
	memcpy(cursor, ourMaterial->GetName().c_str(), bytes);
	cursor += bytes;

	// Store Mat. Path
	bytes = sizeof(char) * ourMaterial->GetPath().length();
	memcpy(cursor, ourMaterial->GetPath().c_str(), bytes);
	cursor += bytes;

	return size;
}

void MaterialLoader::Load(R_Material* myNewMaterial, char* buffer)
{

	char* cursor = buffer;
	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	// Load Color
	bytes = sizeof(uint) * ranges[0];
	Color* newMatColor = new Color();
	memcpy(newMatColor, cursor, bytes);
	cursor += bytes;

	myNewMaterial->SetColor(*newMatColor);
	RELEASE(newMatColor);

	// Load Name
	bytes = sizeof(uint);
	int textureid = -1;
	memcpy(&textureid, cursor, bytes);
	cursor += bytes;

	// Load Name
	bytes = ranges[3] * sizeof(char);
	std::string name;
	name.resize(ranges[2]);
	memcpy(&name.at(0), cursor, bytes);
	cursor += bytes;

	myNewMaterial->SetNameAndPath(name.c_str(), "");

	//LoadTexture
	if (textureid > -1)
	{
		Resource* texture = App->resources->RequestResource(textureid);

		if (texture != nullptr)
		{
			myNewMaterial->SetTextureId(texture->GetUid());
		}
		else
		{
			LOG("Could not load texture... resource id %i", textureid);
		}
	}

}
