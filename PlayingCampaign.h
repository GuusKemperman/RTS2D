#pragma once
#include "Scene.h"

class Player;
class Opponent;
class Army;

class PlayingCampaign : 
	public Scene
{
public:
	PlayingCampaign(Game& game, const string& levelName);
	~PlayingCampaign();

	void Load() override;
	void Tick() override;
	void Unload() override;
	
private:
	void ReturnToMainMenu(const string&);
	void TogglePause(const string&);

	void SpawnDemo1() const;
	void SpawnDemo2() const;
	void SpawnDemo3() const;
	void SpawnDemo4() const;
	void SpawnDemo5() const;

	unique_ptr<Army> playerArmy{}, enemyArmy{};

	string levelName;
	bool paused{};
	bool gameOver{};
};

