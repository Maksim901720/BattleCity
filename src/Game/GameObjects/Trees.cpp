#include "Trees.h"

#include "../../Resource/ResourceManager.h"
#include "../../Renderer/Sprite.h"

Trees::Trees(const glm::vec2& position,
	const glm::vec2& size, const float rotation, const float layer) : 
	IGameObject(IGameObject::EObjectType::Ice, position, size, rotation, layer),
	m_sprite(ResourceManager::getSprite("trees")),
	m_blockOffsets { glm::vec2(0.f, m_size.y / 2.f),
					 glm::vec2(m_size.x / 2.f, m_size.y / 2.f),
					 glm::vec2(0.f, 0.f),
					 glm::vec2(m_size.x / 2.f, 0.f) }
{

}

void Trees::renderBrick(const EBlockLocation eBlockLocation) const
{
	m_sprite->render(m_position + m_blockOffsets[static_cast<size_t>(eBlockLocation)], m_size / 2.f, m_rotation, m_layer);
}

void Trees::render() const
{
	renderBrick(EBlockLocation::TopLeft);
	renderBrick(EBlockLocation::TopRight);
	renderBrick(EBlockLocation::BottomLeft);
	renderBrick(EBlockLocation::BottomRight);
}
