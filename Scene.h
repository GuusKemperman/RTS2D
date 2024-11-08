#pragma once

namespace Tmpl8
{
	class Game;
}
class Camera;
class EntityManager;
class TileMap;
class Quadtree;
class UICanvas;

class Scene
{
public:
	Scene(Game& game, const string& type);
	~Scene();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	virtual void Load();
	virtual void Tick();
	virtual void Unload();

	Game& game;
	const string type;

	unique_ptr<Camera> camera;
	unique_ptr<EntityManager> entityManager;
	unique_ptr<TileMap> tileMap;
	unique_ptr<Quadtree> quadTree;
	unique_ptr<UICanvas> canvas;
};

