#ifndef PLAYER_H
#define PLAYER_H

class Entity;
class Shop;
class UpgradeManager;

struct PauseGame;
struct ResetGame;

class Player
{
public:
	static Player& GetInstance();
	~Player();

	// Later player, and all its components, will be made in this class.
	// But for now as not to break testScenes and because the task was Upgrades
	// player is set in main.
	void SetPlayer(Entity* player);

	Entity* GetPlayer();
	// Get the manager so we can use it in places such as Shop or RangeComponent.
	UpgradeManager* GetUpgradeManager();
	Shop* GetShop();
	void IsInShop(bool value);
	bool IsPlayerAlive();
	void SetPlayerAlive(bool alive);

private:
	Player();
	Entity* m_pPlayer;
	bool m_IsInShop = false;
	bool m_IsAlive = true;

	// Player entitys upgrade manager
	UpgradeManager* m_pUpgradeManager = nullptr;
	Shop* m_pShop;

	int m_UpgradeApplied = 0;

	void onResetGame(ResetGame* evnt);
};

#endif // !PLAYER_H
