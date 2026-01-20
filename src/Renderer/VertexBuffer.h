#pragma once

#include <glad/glad.h>

namespace Renderer {

	class VertexBuffer {
	public:
		VertexBuffer();
		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) noexcept;
		VertexBuffer(VertexBuffer&&) noexcept;

		void init(const void* data, const unsigned int size);
		void update(const void* data, const unsigned int size) const;
		void bind() const;
		void bindun() const;

	private:
		GLuint m_id;
	};
}