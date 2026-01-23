#include "Texture2D.h"

namespace RendererEngine {

	Texture2D::Texture2D(const GLuint width, const GLuint height, const unsigned char* data, const int channels,
		const GLenum filter, const GLenum wrapMode)
		: m_width(width), m_height(height)
	{
		switch (channels) {
		case 4: m_mode = GL_RGBA; break;
		case 3: m_mode = GL_RGB; break;
		default: m_mode = GL_RGBA; 
		}

		// генерируем 1 текстуру
		glGenTextures(1, &m_ID);
		// привязывам текстуру к слоту 2D, делая её текущей
		glBindTexture(GL_TEXTURE_2D, m_ID);
		// явно подключаем текстуру к нулевому слоту(происходит по умолчанию)
		glActiveTexture(GL_TEXTURE0);
		// загружаем данные в память GPU(используем уровень mipMap 0)
		glTexImage2D(GL_TEXTURE_2D, 0, m_mode, m_width, m_height, 0, m_mode, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);   // режим отображение по Ox
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);   // режим отображения по Oy
		// способ фильрации пикселей(в данном случае выбираем ближайший пискель, а не усредняем)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glGenerateMipmap(GL_TEXTURE_2D);  // генерируем текстуры разных уровней mipMap

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D& Texture2D::operator=(Texture2D&& texture2D) noexcept
	{
		glDeleteTextures(1, &m_ID);
		m_ID = texture2D.m_ID;
		texture2D.m_ID = 0;
		m_mode = texture2D.m_mode;
		m_width = texture2D.m_width;
		m_height = texture2D.m_height;
		return *this;
	}

	Texture2D::Texture2D(Texture2D&& texture2D) noexcept
	{
		m_ID = texture2D.m_ID;
		texture2D.m_ID = 0;
		m_mode = texture2D.m_mode;
		m_width = texture2D.m_width;
		m_height = texture2D.m_height;
	}

	Texture2D::~Texture2D()
	{
		// удаляем текстуру с видеокарты
		glDeleteTextures(1, &m_ID);
	}

	void Texture2D::bind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture2D::addSubTexture(const std::string& name, const glm::vec2& leftBottomUV, const glm::vec2& rightToUV)
	{
		m_subTextures.emplace(name, subTexture2D(leftBottomUV, rightToUV));
	}

	const Texture2D::subTexture2D& Texture2D::getSubTexture(const std::string& name) 
	{
		auto it = m_subTextures.find(name);
		if (it != m_subTextures.end())
			return it->second;
		const static subTexture2D defaultSubTexture;
		return defaultSubTexture;
	}

}