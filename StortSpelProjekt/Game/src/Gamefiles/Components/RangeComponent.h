#ifndef RANGE_COMPONENT_H
#define RANGE_COMPONENT_H

#include "../ECS/Components/Component.h"

class SceneManager;
class Scene;
class Model;
struct MouseClick;
namespace component
{
	class RangeComponent : public Component
	{
	public:
		RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, int damage = 1, float velocity = 2.5);
		~RangeComponent();

		void Attack(MouseClick* event);

	private:
		int m_Damage;
		float m_Velocity;
		float m_Scale = 0.05;
		int m_NrOfProjectiles = 0;

		Model* m_pModel = nullptr;
		SceneManager* m_pSceneMan = nullptr;
		Scene* m_pScene = nullptr;

	};
}


#endif