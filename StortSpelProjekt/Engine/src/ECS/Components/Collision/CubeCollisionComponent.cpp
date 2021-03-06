#include "stdafx.h"
#include "CubeCollisionComponent.h"
#include "../Renderer/Transform.h"
#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>

component::CubeCollisionComponent::CubeCollisionComponent(Entity* parent, double mass, double halfWidth, double halfHeight, double halfDepth, double friction, double restitution, bool canFall) :
	CollisionComponent(parent, mass, friction, restitution, canFall)
{
	m_pShape = new btBoxShape({ halfWidth,halfHeight, halfDepth });
}

component::CubeCollisionComponent::~CubeCollisionComponent()
{
	delete m_pShape;
}

double component::CubeCollisionComponent::GetDistanceToBottom() const
{
	return dynamic_cast<btBoxShape*>(m_pShape)->getHalfExtentsWithMargin().y();
}
