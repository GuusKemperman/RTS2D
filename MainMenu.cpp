#include "precomp.h"
#include "MainMenu.h"

#include "Army.h"
#include "Opponent.h"
#include "Chopper.h"
#include "TransformComponent.h"
#include "SmallTank.h"
#include "HugeTank.h"
#include "UICanvas.h"
#include "DynamicCursor.h"
#include "InputManager.h"
#include "Camera.h"
#include "UIButton.h"
#include "PlayingCampaign.h"
#include "Quadtree.h"
#include "TimeManager.h"
#include "TileMap.h"

MainMenu::MainMenu(Game& game) :
	Scene(game, "mainmenu")
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Load()
{
	Scene::Load();
	
	tileMap->Load("big");

	std::function<void(const string&)> func = bind(&MainMenu::LoadDemo, this, placeholders::_1);

	for (int i = 1; i <= 5; i++)
	{
		string buttonName = "demo" + to_string(i);
		canvas->GetElement<UIButton>(buttonName).AddCallback(func);
	}

	canvas->GetElement<UIButton>("exit").AddCallback(func);

	army1 = make_unique<Army>(*this, ArmyId::player);
	army2 = make_unique<Army>(*this, ArmyId::enemy);

	entityManager->AddEntity<Opponent>(*army1);
	entityManager->AddEntity<Opponent>(*army2);



}

void MainMenu::Tick()
{
	canvas->Tick();
	entityManager->TickEntities();

	camera->AdjustDesiredPosition(camDir * camMoveSpeed * TimeManager::GetDeltaTime());

	float currentTime = TimeManager::GetTotalTimePassed();
	float timeSinceLastDirChanged = currentTime - timeCamDirChanged;
	if (timeSinceLastDirChanged >= timeBetweenCamDirChange)
	{
		camera->SetDesiredCentre(RandomCameraPosition());
		camDir = AngleToFloat2(Rand(360.0f));
		timeCamDirChanged = currentTime;
		ReplenishArmies();
	}

	camera->DrawFrame();
}

void MainMenu::Unload()
{
	Scene::Unload();
}

void MainMenu::LoadDemo(const string& demoName)
{
	unique_ptr<Scene> request{};
	if (demoName != "exit")
	{
		request = make_unique<PlayingCampaign>(game, demoName);
	}


	game.RequestLoadTo(move(request));

}

float2 MainMenu::RandomCameraPosition() const
{
	const BoundingBox& maxBounds = quadTree->GetMaxBounds();
	return maxBounds.ComputeRandomPointInsideBox(minCamDistFromEdges);
}

void MainMenu::ReplenishArmies() const
{
	const BoundingBox& maxBounds = quadTree->GetMaxBounds();
	Army* armies[2]{ army1.get(), army2.get() };

	const int numOfUnitsXY = static_cast<int>(sqrt(static_cast<double>(desiredArmySize)));
	const float spacing = 70.0f;

	BoundingBox worldBox = quadTree->GetMaxBounds();
	BoundingBox spawnBox{ float2{0.0f}, float2{spacing} };

	for (int armyNum = 0; armyNum < 2; armyNum++)
	{
		Army* army = armies[armyNum];
		size_t armySize = army->GetIds().size();

		for (int x = 0; x < numOfUnitsXY && armySize < desiredArmySize; x++)
		{
			for (int y = 0; y < numOfUnitsXY && armySize < desiredArmySize; y++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + spacing, static_cast<float>(y) * spacing + spacing);

				if (armyNum == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				// Can spawn
				spawnBox.SetTopLeft(spawnPos);

				if (!quadTree->Query(spawnBox, allLayersMask).empty())
				{
					continue;
				}

				if (rand() % 2 == 0)
				{
					army->AddToArmy<SmallTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				else
				{
					army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				armySize++;
			}
		}
		
	}
}
