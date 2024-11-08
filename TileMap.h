#pragma once
#include "Drawable.h"

class Scene;
class Tileset;

class TileMap :
	public Drawable
{
public:
	TileMap(Scene& scene);
	~TileMap();
	
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;

	void Draw(Surface* target, const Camera* camera) const override;

	void Load(const string& mapName);

	uint2 WorldToTile(float2 worldPos) const;
	float2 TileToWorld(uint2 tilePos) const;

	uint TileSizeWorld() const;
	uint TileSizeScreen() const;

private:
	Scene& scene;

	vector<UINT8> data{};
	uint2 sizeInTiles{};

	float2 worldSize{};

	unique_ptr<Tileset> tileset;
};

