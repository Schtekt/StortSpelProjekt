#ifndef ENEMYCOMPONENT_H
#define ENEMYCOMPONENT_H
#include "../ECS/Components/Component.h"
#include "../Misc/EngineRand.h"

class EnemyFactory;
struct Death;
struct SceneChange;

namespace component
{
	class EnemyComponent : public Component
	{
	public:
		EnemyComponent(Entity* parent, EnemyFactory* factory);
		virtual ~EnemyComponent();

		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();
		void SetRandSeed(unsigned long seed);

	private:
		EnemyFactory* m_pFactory;
		double m_TimeSinceLastGrunt = 0.0;
		EngineRand m_Rand;
		bool m_Alive;

		void death(Death* evnt);
		void sceneChange(SceneChange* evnt);
	};
}
#endif
