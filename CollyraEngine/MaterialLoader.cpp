#include "MaterialLoader.h"
#include "TextureLoader.h"

#include "Application.h"
#include "M_FileManager.h"
#include "C_Material.h"

#include "Color.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

void MaterialLoader::Import(const aiMaterial* material, C_Material* myNewMaterial,const char* matName, const char* meshPath)
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
		uint loadTexture = TextureLoader::Import(pathRelativeToFbx.c_str());

		if (loadTexture == 0)
		{
			LOG("Checking if texture exists in Textures Folder....");
			std::string textureName = "";
			std::string textureExtension = "";
			App->physFS->SplitFilePath(path.C_Str(), nullptr, nullptr, &textureName, &textureExtension);
			std::string defaultPath = ASSETS_TEXTYRES_PATH;
			defaultPath += "/" + textureName + "." + textureExtension;
			loadTexture = TextureLoader::Import(defaultPath.c_str());
		}

		//SAVE---
		if (loadTexture > 0)
		{
			compressedTextureSize = TextureLoader::Save(&compressedTextureBuffer, loadTexture);
			if (compressedTextureSize > 0)
			{
				texturePath.append(textureName).append(".dds");
				App->physFS->Save(texturePath.c_str(), compressedTextureBuffer, compressedTextureSize);
			}
		}

		//LOAD----
		if (loadTexture != 0)
		{
			LOG("Texture loaded! ID: %i", loadTexture);

			myNewMaterial->SetTexture(loadTexture);
			myNewMaterial->SetTextureNameAndPath(textureName.c_str(), texturePath.c_str());
		}
		else
		{
			LOG("Material's texture found but could not be loaded. Texture not found in path given by the importer: %s", pathRelativeToFbx.c_str());
		}
	}
}

std::string MaterialLoader::Save(const C_Material* ourMaterial)
{
	//Color - Text Name - Text Path - Name - Path
	//TODO: Change material name + path -> UID
	std::string path = LIBRARY_MATERIALS + ourMaterial->GetMaterialName() + ".collmat";

	uint ranges[5] = { 4,ourMaterial->GetTextureName().length(),ourMaterial->GetTexturePath().length(),
		ourMaterial->GetMaterialName().length(),path.length() };
	uint size = sizeof(ranges) + sizeof(float) * 4 + 
		ourMaterial->GetMaterialName().length() + path.length() +
		ourMaterial->GetTextureName().length() +  ourMaterial->GetTexturePath().length();

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store Color
	bytes = sizeof(float) * 4;
	memcpy(cursor, &ourMaterial->GetColor(), bytes);
	cursor += bytes;

	// Store Text. Name
	bytes = sizeof(char) * ourMaterial->GetTextureName().length();
	memcpy(cursor, ourMaterial->GetTextureName().c_str(), bytes);
	cursor += bytes;

	// Store Text. Path
	bytes = sizeof(char) * ourMaterial->GetTexturePath().length();
	memcpy(cursor, ourMaterial->GetTexturePath().c_str(), bytes);
	cursor += bytes;

	// Store Mat. Name
	bytes = sizeof(char) * ourMaterial->GetMaterialName().length();
	memcpy(cursor, ourMaterial->GetMaterialName().c_str(), bytes);
	cursor += bytes;

	// Store Mat. Path
	bytes = sizeof(char) * path.length();
	memcpy(cursor, path.c_str(), bytes);
	cursor += bytes;

	App->physFS->Save(path.c_str(), fileBuffer, size);
	RELEASE_ARRAY(fileBuffer);

	return path;
}

void MaterialLoader::Load(C_Material* myNewMaterial, const char* path)
{
	char* buffer = nullptr;
	App->physFS->Load(path, &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;
		// amount of indices / vertices / normals / texture_coords / AABB
		uint ranges[5];
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
		bytes = ranges[1] * sizeof(char);
		std::string textName;
		textName.resize(ranges[1]);
		memcpy(&textName.at(0), cursor, bytes);
		cursor += bytes;

		// Load Path
		bytes =  ranges[2];
		std::string textPath;
		textPath.resize(ranges[2]);
		memcpy(&textPath.at(0), cursor, bytes);
		cursor += bytes;

		myNewMaterial->SetTextureNameAndPath(textName.c_str(), textPath.c_str());

		// Load Name
		bytes = ranges[3] * sizeof(char);
		std::string name;
		name.resize(ranges[3]);
		memcpy(&name.at(0), cursor, bytes);
		cursor += bytes;

		// Load Path
		bytes = ranges[4];
		std::string path;
		path.resize(ranges[4]);
		memcpy(&path.at(0), cursor, bytes);
		cursor += bytes;

		myNewMaterial->SetNameAndPath(name.c_str(), path.c_str());

		char* textureBuffer = nullptr;
		uint textureSize = App->physFS->Load(myNewMaterial->GetTexturePath().c_str(), &textureBuffer);

		//LoadTexture
		if (textureSize > 0)
		{
			myNewMaterial->SetTexture(TextureLoader::Load(textureBuffer, textureSize));
		}

		RELEASE_ARRAY(buffer);
	}
	else
	{
		LOG("Coudn't load mesh from internal library...");
	}
}
