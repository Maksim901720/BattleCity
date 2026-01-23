#include "ResourceManager.h"
#include "../Renderer/ShaderProgram.h"
#include "../Renderer/Texture2D.h"
#include "../Renderer/Sprite.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

// используем библиоткеу stb_image, загружем только png файлы
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
ResourceManager::TexturesMaps ResourceManager::m_textures;
ResourceManager::SpritesMaps ResourceManager::m_sprites;
std::string ResourceManager::m_path;
std::vector<std::vector<std::string>> ResourceManager::m_levels;

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

std::shared_ptr<RendererEngine::ShaderProgram> ResourceManager::loadShaderProgram(const std::string& shaderName,
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

	std::shared_ptr<RendererEngine::ShaderProgram> &newShader = 
		m_shaderPrograms.emplace(shaderName, std::make_shared<RendererEngine::ShaderProgram>(vertexString, fragmentString)).first->second;
	if (newShader->isCompiled())
		return newShader;
	else {
		std::cerr << "Can't load shader program:\n" <<
			"Vertex: " << vertexPath << "\n" << 
			"Fragment: " << fragmentPath << std::endl;
		return nullptr;
	}
}

std::shared_ptr<RendererEngine::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName)
{
	ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
	if (it != m_shaderPrograms.end())
		return it->second;
	std::cerr << "Can't find the shader program: " << shaderName << std::endl;
	return nullptr;
}

std::shared_ptr<RendererEngine::Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath)
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

	std::shared_ptr<RendererEngine::Texture2D> newTexture = m_textures.emplace(textureName, std::make_shared<RendererEngine::Texture2D>(width, height, 
									pixels, channels, GL_NEAREST, GL_CLAMP_TO_EDGE)).first->second;

	// очищаем выделенные ресурсы для пикселей
	stbi_image_free(pixels);
	return newTexture;
}

std::shared_ptr<RendererEngine::Texture2D> ResourceManager::getTexture(const std::string& textureName)
{
	TexturesMaps::const_iterator it = m_textures.find(textureName);
	if (it != m_textures.end())
		return it->second;
	std::cerr << "Can't find the texture: " << textureName << std::endl;
	return nullptr;
}

std::shared_ptr<RendererEngine::Sprite> ResourceManager::loadSprite(const std::string& spriteName, const std::string& textureName, 
	const std::string& shaderName, const std::string &subTextureName)
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
	 
	std::shared_ptr<RendererEngine::Sprite> newSprite = m_sprites.emplace(spriteName, std::make_shared<RendererEngine::Sprite>(pTexture, 
																															subTextureName,
																															pShader)).first->second;

	return newSprite;
}

std::shared_ptr<RendererEngine::Sprite> ResourceManager::getSprite(const std::string& spriteName)
{
	SpritesMaps::const_iterator it = m_sprites.find(spriteName);
	if (it != m_sprites.end())
		return it->second;
	std::cerr << "Can't find the texture: " << spriteName << std::endl;
	return nullptr;
}

std::shared_ptr<RendererEngine::Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName,
																	   const std::string& texturePath,
																	   std::vector<std::string> subTextures,
																	   const unsigned int subTextureWidth,
																	   const unsigned int subTextureHeigth)
{
	const float indent = 0.01f;  // отступ от углов subTexture во внутрь, для решения проблемы с 
								 // вылезанием subTexture в соседнюю мз-за округления float
								 // (пиксели из-за маленького значения теряться не будут)
	auto pTexture = loadTexture(textureName, texturePath);
	if (pTexture) {
		const unsigned int textureWidth = pTexture->width();
		const unsigned int textureHeight = pTexture->height();
		unsigned int currentTextureOffsetX = 0;
		unsigned int currentTextureOffsetY = textureHeight;

		for (const auto& currentSubTextureName : subTextures) {
			glm::vec2 leftBottomUV((currentTextureOffsetX + indent) / textureWidth,
				static_cast<float>(currentTextureOffsetY - subTextureHeigth + indent) / textureHeight);
			glm::vec2 rightToUV((currentTextureOffsetX + subTextureWidth - indent) / textureWidth,
				static_cast<float>(currentTextureOffsetY - indent) / textureHeight);
			
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

void ResourceManager::unloadAllResources()
{
	m_shaderPrograms.clear();
	m_textures.clear();
	m_sprites.clear();
}

bool ResourceManager::loadJSONResources(const std::string& JSONpath)
{
	const std::string JSONString = getFileString(JSONpath);
	if (JSONString.empty()) {
		std::cerr << "No JSON resources file" << std::endl;
		return false;
	}

	// парсим JSON объект из файла
	rapidjson::Document document;
	rapidjson::ParseResult parseResult = document.Parse(JSONString.c_str());
	if (!parseResult) {
		std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code()) 
			<< " (" << parseResult.Offset() << ")" << std::endl;
		std::cerr << "In JSON file: " << JSONpath << std::endl;
		return false;
	}

	// достаём шейдерные программы
	auto shadersIt = document.FindMember("shaders");
	if (shadersIt != document.MemberEnd()) {
		for (const auto& currentShader : shadersIt->value.GetArray()) {
			const std::string name = currentShader["name"].GetString();
			const std::string filePath_v = currentShader["filePath_v"].GetString();
			const std::string filePath_f = currentShader["filePath_f"].GetString();

			loadShaderProgram(name, filePath_v, filePath_f);
		}
	}

	// достаём текстурный атлас
	auto textureAtlasesIt = document.FindMember("textureAtlases");
	if (textureAtlasesIt != document.MemberEnd()) {
		for (const auto& currentTextureAtlas : textureAtlasesIt->value.GetArray()) {
			const std::string name = currentTextureAtlas["name"].GetString();
			const std::string filePath = currentTextureAtlas["filePath"].GetString();
			const unsigned int subTextureWidth = currentTextureAtlas["subTextureWidth"].GetUint();
			const unsigned int subTextureHeight = currentTextureAtlas["subTextureHeight"].GetUint();

			const auto subTexturesArray = currentTextureAtlas["subTextures"].GetArray();
			std::vector<std::string> subTextures;
			subTextures.reserve(subTexturesArray.Size());
			for (const auto& currentSubTexture : subTexturesArray) {
				subTextures.emplace_back(currentSubTexture.GetString());
			}

			loadTextureAtlas(name, filePath, subTextures, subTextureWidth, subTextureHeight);
		}
	}

	// достаём спрайты
	auto spritesIt = document.FindMember("sprites");
	if (spritesIt != document.MemberEnd()) {
		for (const auto& currentSprite : spritesIt->value.GetArray()) {
			const std::string name = currentSprite["name"].GetString();
			const std::string textureAtlas = currentSprite["textureAtlas"].GetString();
			const std::string shader = currentSprite["shader"].GetString();
			const std::string subTextureName = currentSprite["initialSubTexture"].GetString();

			auto pSprites = loadSprite(name, textureAtlas, shader, subTextureName);
			if (!pSprites)
				continue;

			auto framesIt = currentSprite.FindMember("frames");
			if (framesIt != currentSprite.MemberEnd()) {
				const auto framesArray = framesIt->value.GetArray();
				std::vector<RendererEngine::Sprite::FrameDescription> framesDescriptions;
				framesDescriptions.reserve(framesArray.Size());

				for (const auto& currentFrameDescription : framesArray) {
					const std::string subTextureStr = currentFrameDescription["subTexture"].GetString();
					const double duration = currentFrameDescription["duration"].GetDouble();
					const auto pTextureAtlas = getTexture(textureAtlas);
					const auto pSubTexture = pTextureAtlas->getSubTexture(subTextureStr);
					framesDescriptions.emplace_back(pSubTexture.leftBottomUV, pSubTexture.rightTopUV, duration);
				}
				pSprites->insertFrames(std::move(framesDescriptions));
			}
		}
	}
	
	// достаём карту
	auto levelsIt = document.FindMember("levels");
	if (levelsIt  != document.MemberEnd()) {
		for (const auto& currentLevel : levelsIt->value.GetArray()) {
			const auto description = currentLevel["description"].GetArray();
			std::vector<std::string> levelRows;
			levelRows.reserve(description.Size());
			size_t maxLength = 0;
			for (const auto& currentRow : description) {
				levelRows.emplace_back(currentRow.GetString());
				if (maxLength < levelRows.back().length())
					maxLength = levelRows.back().length();
			}

			for (auto& currentRow : levelRows) {
				while (currentRow.length() < maxLength)
					currentRow.append("D");
			}
			m_levels.emplace_back(std::move(levelRows));
		}
	}

	return true;
}
