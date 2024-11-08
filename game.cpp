#include "precomp.h"
#include "PlayerData.h"
#include "game.h"
#include "PlayingCampaign.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "MainMenu.h"

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	static PlayerData data; // By keeping this instance alive, the Data stays loaded.
	data.Save(); // Just fixes formatting in the file.

	RequestLoadTo(move(make_unique<MainMenu>(*this)));
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
bool Game::Tick( float deltaTime )
{
	//static Timer t;

	if (requestLoadTo != nullptr)
	{
		SwitchToRequestedScene();
		requestLoadTo = nullptr;
	}

	TimeManager::UpdateDeltaTime(deltaTime);

	activeScene->Tick();
	
	InputManager::Inst().Tick();

	//static float avgTime = 1.0f;
	//avgTime = (t.elapsed() * 1000.0f) * 0.01f + avgTime * 0.99f;
	//printf("%f\n", avgTime);
	//t.reset();
	return gameRunning;
}

void Game::SwitchToRequestedScene()
{
	if (activeScene != nullptr)
	{
		activeScene->Unload();
	}

	activeScene = move(requestLoadTo);

	activeScene->Load();
}

Tmpl8::Game::Game()
{
}

Tmpl8::Game::~Game()
{
}

void Game::MouseUp(int button)
{
	InputManager::Inst().MouseUp(button);
}

void Game::MouseDown(int button)
{
	InputManager::Inst().MouseDown(button);
}

void Game::MouseMove(int x, int y)
{
	InputManager::Inst().MouseMove(x, y);
}

void Game::MouseWheel(float y)
{
	InputManager::Inst().MouseWheel(y);
}

void Game::KeyUp(int key)
{
	InputManager::Inst().KeyUp(key);
}

void Game::KeyDown(int key)
{
	InputManager::Inst().KeyDown(key);
}

void Tmpl8::Game::RequestLoadTo(unique_ptr<Scene> scene)
{
	if (scene == nullptr)
	{
		gameRunning = false;
		return;
	}
	requestLoadTo = move(scene);
}
