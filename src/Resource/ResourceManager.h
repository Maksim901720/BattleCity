#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

namespace RendererEngine 
{
	class ShaderProgram;
	class Texture2D;
	class Sprite;
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

	static std::shared_ptr<RendererEngine::ShaderProgram> loadShaderProgram(const std::string &shaderName, 
		const std::string& vertexPath, const std::string& fragmentPath);
	static std::shared_ptr<RendererEngine::ShaderProgram> getShaderProgram(const std::string& shaderName);

	static std::shared_ptr<RendererEngine::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
	static std::shared_ptr<RendererEngine::Texture2D> getTexture(const std::string& textureName);

	static std::shared_ptr<RendererEngine::Sprite> loadSprite(const std::string& spriteName, const std::string &textureName, const std::string &shaderName,
				const std::string& subTextureName = "default");
	static std::shared_ptr<RendererEngine::Sprite> getSprite(const std::string& spriteName);

	static std::shared_ptr<RendererEngine::Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName,
																				const std::string& texturePath,
																				std::vector<std::string> subTextures,
																				const unsigned int subTextureWidth,
																				const unsigned int subTextureHeigth);

	static bool loadJSONResources(const std::string& JSONpath);

	static const std::vector<std::vector<std::string>>& getLevels() { return m_levels; }

private:

	static std::string getFileString(const std::string& relativeFilePath);

	using ShaderProgramsMap = std::map<const std::string, std::shared_ptr<RendererEngine::ShaderProgram>>;
	static ShaderProgramsMap m_shaderPrograms;

	using TexturesMaps = std::map<const std::string, std::shared_ptr<RendererEngine::Texture2D>>;
	static TexturesMaps m_textures;

	using SpritesMaps = std::map<const std::string, std::shared_ptr<RendererEngine::Sprite>>;
	static SpritesMaps m_sprites;

	static std::vector<std::vector<std::string>> m_levels;
	
	static std::string m_path;
};
