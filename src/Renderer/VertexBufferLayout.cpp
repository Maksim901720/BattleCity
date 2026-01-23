#include "VertexBufferLayout.h"

namespace RendererEngine {
	VertexBufferLayout::VertexBufferLayout()
		: m_stride(0)
	{

	}

	void VertexBufferLayout::reserveElements(const int count)
	{
		m_layoutElements.reserve(count);
	}

	void VertexBufferLayout::addElementLayoutFloat(const unsigned int count, const bool normalized)
	{
		m_layoutElements.emplace_back(static_cast<GLint>(count), GL_FLOAT, static_cast<GLboolean>(normalized), 
				static_cast<unsigned int>(count * sizeof(GLfloat)));
		m_stride += m_layoutElements.back().size;
	}
}