#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

namespace Renderer 
{
	class ShaderProgram;
	class Texture2D;
	class Sprite;
	class AnimatedSprite;
}

class ResourceManager {
public:
	static void setExacutablePath(const std::string& exacutablePath);
	static void unloadAllResources();

	~ResourceManager() = delete;
	ResourceManager() = delete;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;
	ResourceManager(ResourceManager&&) = delete;

	static std::shared_ptr<Renderer::ShaderProgram> loadShaderProgram(const std::string &shaderName, 
		const std::string& vertexPath, const std::string& fragmentPath);
	static std::shared_ptr<Renderer::ShaderProgram> getShaderProgram(const std::string& shaderName);

	static std::shared_ptr<Renderer::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
	static std::shared_ptr<Renderer::Texture2D> getTexture(const std::string& textureName);

	static std::shared_ptr<Renderer::Sprite> loadSprite(const std::string& spriteName, const std::string &textureName, const std::string &shaderName,
				const unsigned int spriteWidth, const unsigned int spriteHeight, const std::string& subTextureName = "default");
	static std::shared_ptr<Renderer::Sprite> getSprite(const std::string& spriteName);

	static std::shared_ptr<Renderer::Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName,
																				const std::string& texturePath,
																				std::vector<std::string> subTextures,
																				const unsigned int subTextureWidth,
																				const unsigned int subTextureHeigth);

	static std::shared_ptr<Renderer::AnimatedSprite> loadAnimatedSprite(const std::string& spriteName,
																 const std::string& textureName, 
																 const std::string& shaderName,
																 const unsigned int spriteWidth,
															   	 const unsigned int spriteHeight, 
																 const std::string& subTextureName = "default");
	static std::shared_ptr<Renderer::AnimatedSprite> getAnimatedSprite(const std::string& spriteName);

private:

	static std::string getFileString(const std::string& relativeFilePath);

	using ShaderProgramsMap = std::map<const std::string, std::shared_ptr<Renderer::ShaderProgram>>;
	static ShaderProgramsMap m_shaderPrograms;

	using TexturesMaps = std::map<const std::string, std::shared_ptr<Renderer::Texture2D>>;
	static TexturesMaps m_textures;

	using SpritesMaps = std::map<const std::string, std::shared_ptr<Renderer::Sprite>>;
	static SpritesMaps m_sprites;

	using AnimatedSpritesMap = std::map<const std::string, std::shared_ptr<Renderer::AnimatedSprite>>;
	static AnimatedSpritesMap m_animatedSprites;
	
	static std::string m_path;
};
