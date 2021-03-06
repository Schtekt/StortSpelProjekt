#ifndef PLAYERPROJECTILECOLLISIONCATEGORY_H
#define PLAYERPROJECTILECOLLISIONCATEGORY_H
#include "../CollisionCategory.h"

class PlayerProjectileCollisionCategory : public CollisionCategory 
{
public:
	PlayerProjectileCollisionCategory(Entity* parent);
	virtual ~PlayerProjectileCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(PlayerCollisionCategory* other) override;
	void Collide(EnemyCollisionCategory* other) override;
};

#endif