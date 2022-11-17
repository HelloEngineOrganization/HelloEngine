#include "Headers.h"
#include "TextureImporter.h"
#include "IL/il.h"
#include "IL/ilut.h"
#include "Texture.h"
#include "TextureManager.h"
#include "ModuleFiles.h"

std::string TextureImporter::ImportImage(const std::string& fileName, char* buffer, uint size)
{
	std::string ddsFilePath = "Resources/Textures/" + fileName + ".dds";

	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);
	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG("Error loading image: %s", ilutGetString(ilGetError()));
	}

	ILuint imgSize;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	imgSize = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer

	if (imgSize > 0)
	{
		data = new ILubyte[imgSize]; // allocate data buffer
		ilSaveL(IL_DDS, data, imgSize); // Save to buffer with the ilSaveIL function

		ModuleFiles::S_Save(ddsFilePath, (char*)data, imgSize, false);

		RELEASE_ARRAY(data);
	}

	ilDeleteImages(1, &ImgId);
	return ddsFilePath;
}

uint TextureImporter::Load(char* buffer, int size, int* width, int* heigth, std::string&& filename)
{
	//Check if the given texture has been already loaded
	if (TextureManager::usedPaths.find(filename) != TextureManager::usedPaths.end())
	{
		return TextureManager::usedPaths[filename]; // If this texture path was already loaded, return the loaded texture.
	}

	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);
	ilBindImage(ImgId);
	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG("Error loading image: %s", ilutGetString(ilGetError()));
	}

	uint error = ilGetError();

	Texture engineTexture;

	engineTexture.width = ilGetInteger(IL_IMAGE_WIDTH);
	engineTexture.height = ilGetInteger(IL_IMAGE_HEIGHT);
	engineTexture.name = filename;

	ILubyte* bytes = ilGetData();

	glGenTextures(1, &engineTexture.OpenGLID);

	//TODO: Generate mipmaps and use best settings
	glBindTexture(GL_TEXTURE_2D, engineTexture.OpenGLID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, engineTexture.width, engineTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &ImgId);
	glBindTexture(GL_TEXTURE_2D, 0);

	TextureManager::loadedTextures[engineTexture.OpenGLID] = engineTexture; // Add loaded texture inside TextureManager.
	TextureManager::usedPaths[filename] = engineTexture.OpenGLID;

	return engineTexture.OpenGLID;
}

uint TextureImporter::CheckerImage()
{
	//Check if the given texture has been already loaded
	if (TextureManager::usedPaths.find("Checkers") != TextureManager::usedPaths.end())
	{
		return TextureManager::usedPaths["Checkers"]; // If this texture path was already loaded, return the loaded texture.
	}

	GLubyte checkerImage[240][240][4];
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 240; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	uint textureID = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 240, 240,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	Texture engineTexture;
	engineTexture.OpenGLID = textureID;
	engineTexture.name = "Checkers";
	engineTexture.width = engineTexture.height = 240;

	TextureManager::loadedTextures[textureID] = engineTexture; // Add loaded texture inside TextureManager.
	TextureManager::usedPaths["Checkers"] = textureID;

	return textureID;
}