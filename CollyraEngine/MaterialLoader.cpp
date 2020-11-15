#include "MaterialLoader.h"
#include "TextureLoader.h"

#include "Application.h"
#include "M_FileManager.h"
#include "C_Material.h"

#include "Color.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

void MaterialLoader::IImport(const aiMaterial* material, C_Material* myNewMaterial, const char* meshPath)
{
	//Color----
	aiColor4D materialColor;
	if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &materialColor) == aiReturn_SUCCESS)
	{
		myNewMaterial->SetColor(Color{ materialColor.r, materialColor.g, materialColor.b, materialColor.a });
	}

	//Texture---
	aiString path;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{
		std::string loadedPath = "";

		std::string pathRelativeToFbx = path.C_Str();
		std::string relativePath = "";
		std::string textureName = "";
		App->physFS->SplitFilePath(meshPath, nullptr, &relativePath, &textureName, nullptr);
		pathRelativeToFbx = App->physFS->GetProjectPathFromInternalRelative(relativePath, pathRelativeToFbx);
		loadedPath = pathRelativeToFbx;

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
			loadedPath = defaultPath;
		}

		//SAVE---
		if (loadTexture > 0)
		{
			compressedTextureSize = TextureLoader::Save(&compressedTextureBuffer, loadTexture);
			if (compressedTextureSize > 0)
			{
				texturePath.append(textureName);
				App->physFS->Save(texturePath.c_str(), compressedTextureBuffer, compressedTextureSize);
			}
		}

		//LOAD----
		if (loadTexture != 0)
		{
			LOG("Texture loaded! ID: %i", loadTexture);

			std::string materialName = "";
			std::string materialExtension = "";
			App->physFS->SplitFilePath(texturePath.c_str(), nullptr, nullptr, &materialName, &materialExtension);

			myNewMaterial->SetTexture(loadTexture);
			myNewMaterial->SetTextureNameAndPath(std::string(materialName + "." + materialExtension).c_str(), texturePath.c_str());
		}
		else
		{
			LOG("Material's texture found but could not be loaded. Texture not found in path given by the importer: %s", pathRelativeToFbx.c_str());
		}
	}
}

std::string MaterialLoader::Save(const C_Material* ourMaterial)
{
	//Color - Name Length - Name - PathLength - Path
	//TODO: Change material name + path -> UID
	uint ranges[3] = { 4,ourMaterial->GetMaterialName().length(),ourMaterial->GetMaterialPath().length() };
	uint size = sizeof(ranges) + sizeof(float) * 4 + 
		ourMaterial->GetMaterialName().length() * sizeof(char) + ourMaterial->GetMaterialPath().length() *sizeof(char);

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store Color
	bytes = sizeof(float) * 4;
	memcpy(cursor, &ourMaterial->GetColor(), bytes);
	cursor += bytes;

	// Store Name
	bytes = sizeof(char) * ourMaterial->GetMaterialName().length();
	memcpy(cursor, ourMaterial->GetMaterialName().c_str(), bytes);
	cursor += bytes;

	// Store Path
	bytes = sizeof(char) * ourMaterial->GetMaterialPath().length();
	memcpy(cursor, ourMaterial->GetMaterialPath().c_str(), bytes);
	cursor += bytes;

	std::string path = LIBRARY_MATERIALS + ourMaterial->GetMaterialName() + ".collmat";

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
		uint ranges[3];
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
		std::string name;
		name.resize(ranges[1]);
		memcpy(&name.at(0), cursor, bytes);
		cursor += bytes;

		// Load Path
		bytes =  ranges[2];
		std::string path;
		path.resize(ranges[2]);
		memcpy(&path.at(0), cursor, bytes);
		cursor += bytes;

		myNewMaterial->SetTextureNameAndPath(name.c_str(), path.c_str());

		char* textureBuffer = nullptr;
		uint textureSize = App->physFS->Load(myNewMaterial->GetMaterialPath().c_str(), &textureBuffer);

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
