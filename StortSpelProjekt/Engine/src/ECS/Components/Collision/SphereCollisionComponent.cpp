#include "stdafx.h"
#include "SphereCollisionComponent.h"
#include "../Renderer/Transform.h"
component::SphereCollisionComponent::SphereCollisionComponent(Entity* parent, double mass, double radius, double friction, double restitution) :
	CollisionComponent(parent, mass, friction, restitution)
{
	m_pShape = new btSphereShape(radius);
}

component::SphereCollisionComponent::~SphereCollisionComponent()
{
	delete m_pShape;
}

double component::SphereCollisionComponent::GetDistanceToBottom() const
{
	return dynamic_cast<btSphereShape*>(m_pShape)->getRadius();
}