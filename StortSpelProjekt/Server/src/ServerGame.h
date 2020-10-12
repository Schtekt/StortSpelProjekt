#ifndef SERVERGAME_H
#define SERVERGAME_H

#include "SFML/Network.hpp"
#include <vector>

struct ServerEntity
{
	std::string name;
	float3 position;
	double4 rotation;
	double3 velocity;
};

class ServerGame
{
public:
	ServerGame();
	~ServerGame();

	void StartGameState();

	void Update(double dt);

	void UpdateEntity(std::string name, float3 position, double4 rotation, double3 velocity);
	ServerEntity* GetEntity(std::string name);

	void AddEntity(std::string name);
	void RemoveEntity(std::string name);

private:
	int m_FrameCount;

	std::vector<ServerEntity*> m_Entities;
};

#endif
