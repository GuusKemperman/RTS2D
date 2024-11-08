#include "precomp.h"
#include "Scene.h"
#include "Camera.h"
#include "EntityManager.h"
#include "TileMap.h"
#include "Quadtree.h"
#include "PlayerData.h"
#include "game.h"
#include "UICanvas.h"
#include "Entity.h"

Scene::Scene(Game& a_game, const string& a_type) :
	game(a_game),
	type(a_type)
{

}

Scene::~Scene()
{

}

void Scene::Load()
{
	PlayerData data("scenes." + type);

	camera = make_unique<Camera>(game.screen);
	quadTree = make_unique<Quadtree>(*this);
	tileMap = make_unique<TileMap>(*this);
	canvas = make_unique<UICanvas>(*this, data.GetVariableValue("canvas"));

	entityManager = make_unique<EntityManager>(*this);
}

void Scene::Tick()
{
}

void Scene::Unload()
{
	entityManager.reset();

}
