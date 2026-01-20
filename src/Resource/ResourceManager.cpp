#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/Sprite.h"
#include "../Renderer/AnimatedSprite.h"

#include <sstream>
#include <fstream>
#include <iostream>

// используем библиоткеу stb_image, загружем только png файлы
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
ResourceManager::TexturesMaps ResourceManager::m_textures;
ResourceManager::SpritesMaps ResourceManager::m_sprites;
ResourceManager::AnimatedSpritesMap ResourceManager::m_animatedSprites;
std::string ResourceManager::m_path;

void ResourceManager::setExacutablePath(const std::string& executablePath)
{
	// находим в пути последний символ /(или \)
	size_t found = executablePath.find_last_of("/\\");
	m_path = executablePath.substr(0, found);
}

std::string ResourceManager::getFileString(const std::string& relativeFilePath) 
{
	std::ifstream f;
	f.open(m_path + "/" + relativeFilePath.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open()) {
		std::cerr << "Failed to open file: " << relativeFilePath << std::endl;
		return std::string("");
	}

	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::loadShaderProgram(const std::string& shaderName,
	const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexString = getFileString(vertexPath);
	if (vertexString.empty()) {
		std::cerr << "No vertex shader " << std::endl;
		return nullptr;
	}

	std::string fragmentString = getFileString(fragmentPath);
	if (fragmentString.empty()) {
		std::cerr << "No fragment shader " << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::ShaderProgram> &newShader = 
		m_shaderPrograms.emplace(shaderName, std::make_shared<Renderer::ShaderProgram>(vertexString, fragmentString)).first->second;
	if (newShader->isCompiled())
		return newShader;
	else {
		std::cerr << "Can't load shader program:\n" <<
			"Vertex: " << vertexPath << "\n" << 
			"Fragment: " << fragmentPath << std::endl;
		return nullptr;
	}
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName)
{
	ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
	if (it != m_shaderPrograms.end())
		return it->second;
	std::cerr << "Can't find the shader program: " << shaderName << std::endl;
	return nullptr;
}

std::shared_ptr<Renderer::Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath)
{
	int channels = 0;
	int width = 0;
	int height = 0;

	// указываем загружать байты последовательно с нижнего левого угла для дальнейшего использования в OpenGL
	// (по умолчанию начинает с левого верхнего угла)
	stbi_set_flip_vertically_on_load(true);
	// загружем указанную текстуру, получаем её ширину высоту, количество каналов(3 для RGB, 4 для RGBA).
	// Так как мы изначально не знаем количество каналов, указываем 0
	unsigned char *pixels = stbi_load(std::string(m_path + "/" + texturePath).c_str(), &width, &height, &channels, 0);

	if (!pixels) {
		std::cerr << "Can't load texture: " << texturePath << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::Texture2D> newTexture = m_textures.emplace(textureName, std::make_shared<Renderer::Texture2D>(width, height, 
									pixels, channels, GL_NEAREST, GL_CLAMP_TO_EDGE)).first->second;

	// очищаем выделенные ресурсы для пикселей
	stbi_image_free(pixels);
	return newTexture;
}

std::shared_ptr<Renderer::Texture2D> ResourceManager::getTexture(const std::string& textureName)
{
	TexturesMaps::const_iterator it = m_textures.find(textureName);
	if (it != m_textures.end())
		return it->second;
	std::cerr << "Can't find the texture: " << textureName << std::endl;
	return nullptr;
}

std::shared_ptr<Renderer::Sprite> ResourceManager::loadSprite(const std::string& spriteName, const std::string& textureName, 
	const std::string& shaderName, const unsigned int spriteWidth, const unsigned int spriteHeight, const std::string &subTextureName)
{
	auto pTexture = getTexture(textureName);
	if (!pTexture) {
		std::cerr << "Can't find the texture: " << textureName << " for the sprite: " << spriteName << std::endl;
		return nullptr;
	}

	auto pShader = getShaderProgram(shaderName);
	if (!pShader) {
		std::cerr << "Can't find the shared program: " << shaderName << " for the shader sprite: " << spriteName << std::endl;
		return nullptr;
	}
	 
	std::shared_ptr<Renderer::Sprite> newSprite = m_sprites.emplace(spriteName, std::make_shared<Renderer::Sprite>(pTexture, 
									subTextureName,
									pShader, 
									glm::vec2(0.0f, 0.0f), 
									glm::vec2(spriteWidth, spriteHeight), 
									0.f)).first->second;

	return newSprite;
}

std::shared_ptr<Renderer::Sprite> ResourceManager::getSprite(const std::string& spriteName)
{
	SpritesMaps::const_iterator it = m_sprites.find(spriteName);
	if (it != m_sprites.end())
		return it->second;
	std::cerr << "Can't find the texture: " << spriteName << std::endl;
	return nullptr;
}

std::shared_ptr<Renderer::Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName,
																	   const std::string& texturePath,
																	   std::vector<std::string> subTextures,
																	   const unsigned int subTextureWidth,
																	   const unsigned int subTextureHeigth)
{
	auto pTexture = loadTexture(textureName, texturePath);
	if (pTexture) {
		const unsigned int textureWidth = pTexture->width();
		const unsigned int textureHeight = pTexture->height();
		unsigned int currentTextureOffsetX = 0;
		unsigned int currentTextureOffsetY = textureHeight;

		for (const auto& currentSubTextureName : subTextures) {
			glm::vec2 leftBottomUV(static_cast<float>(currentTextureOffsetX) / textureWidth,
				static_cast<float>(currentTextureOffsetY - subTextureHeigth) / textureHeight);
			glm::vec2 rightToUV(static_cast<float>(currentTextureOffsetX + subTextureWidth) / textureWidth,
				static_cast<float>(currentTextureOffsetY) / textureHeight);
			
			pTexture->addSubTexture(currentSubTextureName, leftBottomUV, rightToUV);

			currentTextureOffsetX += subTextureWidth;
			if (currentTextureOffsetX >= textureWidth) {
				currentTextureOffsetX = 0;
				currentTextureOffsetY -= subTextureHeigth;
			}
		}
	}
	return pTexture;
}

std::shared_ptr<Renderer::AnimatedSprite> ResourceManager::loadAnimatedSprite(const std::string& spriteName,
																			  const std::string& textureName,
																			  const std::string& shaderName,
																			  const unsigned int spriteWidth,
																			  const unsigned int spriteHeight,
																			  const std::string& subTextureName)
{
	auto pTexture = getTexture(textureName);
	if (!pTexture) { 
		std::cerr << "Can't find the texture: " << textureName << " for the sprite: " << spriteName << std::endl;
		return nullptr;
	}

	auto pShader = getShaderProgram(shaderName);
	if (!pShader) {
		std::cerr << "Can't find the shared program: " << shaderName << " for the shader sprite: " << spriteName << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::AnimatedSprite> newSprite = m_animatedSprites.emplace(spriteName, std::make_shared<Renderer::AnimatedSprite>(pTexture,
		subTextureName,
		pShader,
		glm::vec2(0.0f, 0.0f),
		glm::vec2(spriteWidth, spriteHeight),
		0.f)).first->second;

	return newSprite;
}

std::shared_ptr<Renderer::AnimatedSprite> ResourceManager::getAnimatedSprite(const std::string& spriteName)
{
	AnimatedSpritesMap::const_iterator it = m_animatedSprites.find(spriteName);
	if (it != m_animatedSprites.end())
		return it->second;
	std::cerr << "Can't find the texture: " << spriteName << std::endl;
	return nullptr;
}

void ResourceManager::unloadAllResources()
{
	m_shaderPrograms.clear();
	m_textures.clear();
	m_sprites.clear();
	m_animatedSprites.clear();
}