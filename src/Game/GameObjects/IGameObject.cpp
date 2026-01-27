#include "IGameObject.h"

IGameObject::IGameObject(const EObjectType objectType, const glm::vec2& position, 
	const glm::vec2& size, const float rotation, const float layer)
	: m_pOwner(nullptr), m_position(position), m_targetPosition(m_position), m_size(size), m_rotation(rotation), m_layer(layer), 
	m_objectType(objectType), m_direction(0., 1.), m_velocity(0.)
{

}

IGameObject::~IGameObject()
{

}

void IGameObject::setVelocity(const double velocity)
{
	m_velocity = velocity;
}

void IGameObject::setOwner(IGameObject* pOwner)
{
	m_pOwner = pOwner;
}