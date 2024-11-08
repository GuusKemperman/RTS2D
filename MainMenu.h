#pragma once
#include "Scene.h"

class Army;
class BoundingBox;

class MainMenu :
    public Scene
{
public:
	MainMenu(Game& game);
	~MainMenu();

	void Load() override;
	void Tick() override;
	void Unload() override;

private:
	void LoadDemo(const string& demoName);
	
	float2 RandomCameraPosition() const;

	void ReplenishArmies() const;

	unique_ptr<Army> army1{}, army2{};
	static constexpr size_t desiredArmySize = 256;

	static constexpr float minCamDistFromEdges = 1000.0f;
	static constexpr float timeBetweenCamDirChange = 10.0f;
	static constexpr float camMoveSpeed = 15.0f;

	float2 camDir{};
	float timeCamDirChanged = -timeBetweenCamDirChange;
};

