#ifndef COMPONENT_H
#define COMPONENT_H

class Entity;
class Renderer;

class Component
{
public:
	Component(Entity* parent);
	virtual ~Component();

	//Use update for logic stuff. It will be using fixed timestep
	virtual void Update(double dt);
	//Use render update for all things rendering. It will be using variable timestep
	virtual void RenderUpdate(double dt);
	//SceneInit
	virtual void OnInitScene() = 0;
	virtual void OnUnInitScene() = 0;

	virtual void Reset();

	Entity* const GetParent() const;

protected:
	Entity* m_pParent = nullptr;
};

#endif
