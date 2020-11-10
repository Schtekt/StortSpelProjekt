#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <map>
#include <string>
#include <vector>

class GameGUI
{
public:
	GameGUI();

	void Update(double dt, Scene* scene);

private:
	int m_OldHealth;
	int m_OldMaxHealth;
	int m_OldMoney;
	float m_OldHealthLength;
	Scene* m_pOldScene;

	void updateHealth(Scene* scene);
	void reset(Scene* scene);
};


#endif // !GAME_GUI_H