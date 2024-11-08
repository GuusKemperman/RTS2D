#include "precomp.h"
#include "SmallTank.h"
#include "Player.h"
#include "Opponent.h"
#include "TileMap.h"
#include "PlayingCampaign.h"
#include "EntityManager.h"
#include "TransformComponent.h"
#include "HugeTank.h"
#include "Army.h"
#include "Camera.h"

#include "UISprite.h"
#include "UICanvas.h"
#include "TimeManager.h"
#include "MySprite.h"
#include "InputManager.h"
#include "DynamicCursor.h"
#include "Chopper.h"
#include "Input.h"
#include "Binding.h"
#include "UIButton.h"
#include "MainMenu.h"
#include "Quadtree.h"

PlayingCampaign::PlayingCampaign(Game& game, const string& a_levelName) :
	Scene(game, "playing"),
	levelName(a_levelName)
{

}

PlayingCampaign::~PlayingCampaign()
{
}

void PlayingCampaign::Load()
{
	Scene::Load();

	playerArmy = make_unique<Army>(*this, ArmyId::player);
	enemyArmy = make_unique<Army>(*this, ArmyId::enemy);

	entityManager->AddEntity<Player>(*playerArmy);
	entityManager->AddEntity<Opponent>(*enemyArmy);

	std::function<void(const string&)> func = bind(&PlayingCampaign::ReturnToMainMenu, this, placeholders::_1);
	canvas->GetElement<UIButton>("returnmainmenu").AddCallback(func);

	func = bind(&PlayingCampaign::TogglePause, this, placeholders::_1);
	canvas->GetElement<UIButton>("returntogame").AddCallback(func);


	if (levelName == "demo1")
	{
		tileMap->Load("default");
		SpawnDemo1();
	}
	else if (levelName == "demo2")
	{
		tileMap->Load("default");
		SpawnDemo2();
	}
	else if (levelName == "demo3")
	{
		tileMap->Load("big");
		SpawnDemo3();
	}
	else if (levelName == "demo4")
	{
		tileMap->Load("huge");
		SpawnDemo4();
	}
	else if (levelName == "demo5")
	{
		tileMap->Load("huge");
		SpawnDemo5();
	}
}

void PlayingCampaign::Tick()
{
	canvas->Tick();
	entityManager->TickEntities();
	camera->DrawFrame();

	UISprite& alert = canvas->GetElement<UISprite>("alertlight");

	float timePassed = TimeManager::GetTotalTimePassed();
	const float fps = 1.0f;
	uint animationOffset = static_cast<uint>(fmodf(timePassed, 2.0f / fps) * fps);
	alert.sprite->SetFrame(animationOffset);

	if (InputManager::GetBinding("pause").IsPressed())
	{
		TogglePause("");
	}

	if (gameOver)
	{
		return;
	}

	if (playerArmy->GetIds().empty())
	{
		canvas->GetElement<UISprite>("defeat").SetActive(true);
		gameOver = true;
	}
	else if (enemyArmy->GetIds().empty())
	{
		canvas->GetElement<UISprite>("victory").SetActive(true);
		gameOver = true;
	}
}

void PlayingCampaign::Unload()
{
	TimeManager::SetTimeScale(1.0f);

	Scene::Unload();
}

void PlayingCampaign::ReturnToMainMenu(const string&)
{
	game.RequestLoadTo(move(make_unique<MainMenu>(game)));
}

void PlayingCampaign::TogglePause(const string&)
{
	paused = !paused;
	TimeManager::SetTimeScale(static_cast<float>(!paused));
	canvas->GetElement<UIElement>("pausemenu").SetActive(paused);

	if (gameOver)
	{
		canvas->GetElement<UIElement>("nodewin").SetActive(!paused);
	}
}

void PlayingCampaign::SpawnDemo1() const
{
	const int numOfUnitsXY = 1;
	const float spacing = 30.0f;
	BoundingBox worldBox = quadTree->GetMaxBounds();

	Army* armies[2]{ playerArmy.get(), enemyArmy.get() };

	for (int x = 0; x < numOfUnitsXY; x++)
	{
		for (int y = 0; y < numOfUnitsXY; y++)
		{

			for (int i = 0; i < 2; i++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + 200.0f, static_cast<float>(y) * spacing + 200.0f);

				if (i == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				Army* army = armies[i];
				army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
			}
		}
	}
}

void PlayingCampaign::SpawnDemo2() const
{
	const int numOfUnitsXY = 10;
	const float spacing = 30.0f;
	BoundingBox worldBox = quadTree->GetMaxBounds();

	Army* armies[2]{ playerArmy.get(), enemyArmy.get() };

	for (int x = 0; x < numOfUnitsXY; x++)
	{
		for (int y = 0; y < numOfUnitsXY; y++)
		{

			for (int i = 0; i < 2; i++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + 200.0f, static_cast<float>(y) * spacing + 200.0f);

				if (i == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				Army* army = armies[i];
				army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);

				if ((x + y * (numOfUnitsXY - 1)) % 2 == 0)
				{
					army->AddToArmy<SmallTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				else
				{
					army->AddToArmy<HugeTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
			}
		}
	}
}

void PlayingCampaign::SpawnDemo3() const
{
	const int numOfUnitsXY = 25;
	const float spacing = 30.0f;
	BoundingBox worldBox = quadTree->GetMaxBounds();

	Army* armies[2]{ playerArmy.get(), enemyArmy.get() };

	for (int x = 0; x < numOfUnitsXY; x++)
	{
		for (int y = 0; y < numOfUnitsXY; y++)
		{

			for (int i = 0; i < 2; i++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + 200.0f, static_cast<float>(y) * spacing + 200.0f);

				if (i == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				Army* army = armies[i];
				army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);

				if ((x + y * (numOfUnitsXY - 1)) % 2 == 0)
				{
					army->AddToArmy<SmallTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				else
				{
					army->AddToArmy<HugeTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
			}
		}
	}
}

void PlayingCampaign::SpawnDemo4() const
{
	const int numOfUnitsXY = 30;
	const float spacing = 30.0f;
	BoundingBox worldBox = quadTree->GetMaxBounds();

	Army* armies[2]{ playerArmy.get(), enemyArmy.get() };

	for (int x = 0; x < numOfUnitsXY; x++)
	{
		for (int y = 0; y < numOfUnitsXY; y++)
		{

			for (int i = 0; i < 2; i++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + 200.0f, static_cast<float>(y) * spacing + 200.0f);

				if (i == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				Army* army = armies[i];
				army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);

				if ((x + y * (numOfUnitsXY - 1)) % 2 == 0)
				{
					army->AddToArmy<SmallTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				else
				{
					army->AddToArmy<HugeTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
			}
		}
	}
}

void PlayingCampaign::SpawnDemo5() const
{
	const int numOfUnitsXY = 32;
	const float spacing = 30.0f;
	BoundingBox worldBox = quadTree->GetMaxBounds();

	Army* armies[2]{ playerArmy.get(), enemyArmy.get() };

	for (int x = 0; x < numOfUnitsXY; x++)
	{
		for (int y = 0; y < numOfUnitsXY; y++)
		{

			for (int i = 0; i < 2; i++)
			{
				float2 spawnPos(static_cast<float>(x) * spacing + 200.0f, static_cast<float>(y) * spacing + 200.0f);

				if (i == 1)
				{
					spawnPos = worldBox.GetBottomRight() - spawnPos;
				}

				Army* army = armies[i];
				army->AddToArmy<Chopper>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);

				if ((x + y * (numOfUnitsXY - 1)) % 2 == 0)
				{
					army->AddToArmy<SmallTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}
				else
				{
					army->AddToArmy<HugeTank>().GetComponent<TransformComponent>()->SetWorldPos(spawnPos);
				}

			}
		}
	}
}
