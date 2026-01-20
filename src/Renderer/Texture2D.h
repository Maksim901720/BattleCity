#pragma once

#include <glad/glad.h>
#include <string>
#include <map>
#include <glm/vec2.hpp>

namespace Renderer {
	class Texture2D {
	public:

		struct subTexture2D {
			glm::vec2 leftBottomUV;
			glm::vec2 rightTopUV;

			subTexture2D(const glm::vec2& _leftBottomUV, const glm::vec2& _rightToUV) : 
				leftBottomUV(_leftBottomUV), rightTopUV(_rightToUV) {}

			subTexture2D() : 
				leftBottomUV(0.0f), rightTopUV(1.0f) {}
		};

		Texture2D(const GLuint width, const GLuint height, const unsigned char *data, const int channels = 4, 
				   const GLenum filter = GL_LINEAR, const GLenum wrapMode = GL_CLAMP_TO_EDGE);
		
		Texture2D() = delete;
		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&& texture2D) noexcept;
		Texture2D(Texture2D&& texture2D) noexcept;
		~Texture2D();

		void addSubTexture(const std::string& name, const glm::vec2& leftBottomUV, const glm::vec2& rightToUV);
		const subTexture2D& getTexture(const std::string& name);
		unsigned int width() const { return m_width; }
		unsigned int height() const { return m_height; }
		void bind() const;
	private:

		GLuint m_ID;
		GLenum m_mode;
		unsigned int m_width, m_height;

		std::map<std::string, subTexture2D> m_subTextures;
	};
}