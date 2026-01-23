#include "BetonWall.h"

#include "../../Resource/ResourceManager.h"
#include "../../Renderer/Sprite.h"

BetonWall::BetonWall(const EBetonWallType eBetonWallType, const glm::vec2& position,
	const glm::vec2& size, const float rotation, const float layer) :
	IGameObject(IGameObject::EObjectType::BetonWall, position, size, rotation, layer),
	m_eCurrentBlockState{ EBetonState::Destroyed,
						  EBetonState::Destroyed,
						  EBetonState::Destroyed,
						  EBetonState::Destroyed },
	m_sprite(ResourceManager::getSprite("betonWall")),
	m_blockOffsets { glm::vec2(0.f, m_size.y / 2.f),
					 glm::vec2(m_size.x / 2.f, m_size.y / 2.f),
					 glm::vec2(0.f, 0.f),
					 glm::vec2(m_size.x / 2.f, 0.f) }
{
	switch (eBetonWallType)
	{
	case EBetonWallType::All:
		m_eCurrentBlockState.fill(EBetonState::Enabled);
		m_colliders.emplace_back(glm::vec2(0), m_size);
		break;
	case EBetonWallType::Top:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBetonState::Enabled;
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(0, m_size.y / 2), m_size);
		break;
	case EBetonWallType::Bottom:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBetonState::Enabled;
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(0), glm::vec2(m_size.x, m_size.y / 2));
		break;
	case EBetonWallType::Left:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBetonState::Enabled;
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(0), glm::vec2(m_size.x / 2, m_size.y));
		break;
	case EBetonWallType::Right:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBetonState::Enabled;
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(m_size.x / 2, 0), m_size);
		break;
	case EBetonWallType::TopLeft:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopLeft)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(0, m_size.y / 2), glm::vec2(m_size.x / 2, m_size.y));
		break;
	case EBetonWallType::TopRight:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::TopRight)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(m_size.x / 2, m_size.y / 2), m_size);
		break;
	case EBetonWallType::BottomLeft:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomLeft)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(0), glm::vec2(m_size.x / 2, m_size.y / 2));
		break;
	case EBetonWallType::BottomRight:
		m_eCurrentBlockState[static_cast<size_t>(EBlockLocation::BottomRight)] = EBetonState::Enabled;
		m_colliders.emplace_back(glm::vec2(m_size.x / 2, 0), glm::vec2(m_size.x, m_size.y / 2));
		break;

	default:
		break;
	}
}

void BetonWall::renderBrick(const EBlockLocation eBlockLocation) const
{
	const EBetonState state = m_eCurrentBlockState[static_cast<size_t>(eBlockLocation)];
	if (state != EBetonState::Destroyed) {
		m_sprite->render(m_position + m_blockOffsets[static_cast<size_t>(eBlockLocation)], m_size / 2.f, m_rotation);
	}
}

void BetonWall::render() const
{
	renderBrick(EBlockLocation::TopLeft);
	renderBrick(EBlockLocation::TopRight);
	renderBrick(EBlockLocation::BottomLeft);
	renderBrick(EBlockLocation::BottomRight);
}

void BetonWall::update(const double delta)
{

}
