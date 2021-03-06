#include "stdafx.h"
#include "CapsuleCollisionComponent.h"

component::CapsuleCollisionComponent::CapsuleCollisionComponent(Entity* parent, double mass, double radius, double height, double friction, double restitution, bool canFall)
	:CollisionComponent(parent, mass, friction, restitution, canFall)
{
	m_pShape = new btCapsuleShape(radius, height);
}

component::CapsuleCollisionComponent::~CapsuleCollisionComponent()
{
	delete m_pShape;
}

double component::CapsuleCollisionComponent::GetDistanceToBottom() const
{
	return static_cast<btCapsuleShape*>(m_pShape)->getHalfHeight() + static_cast<btCapsuleShape*>(m_pShape)->getRadius();
}
