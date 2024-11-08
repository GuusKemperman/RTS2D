#include "precomp.h"
#include "Camera.h"
#include "PlayerData.h"
#include "TileMap.h"
#include "Quadtree.h"
#include "Tileset.h"
#include "MySprite.h"
#include <sstream>
#include "Scene.h"

TileMap::TileMap(Scene& a_scene) :
	Drawable(a_scene, -1000),
	scene(a_scene)
{

}

TileMap::~TileMap()
{
}

void TileMap::Load(const string& mapName)
{
	PlayerData mapData("maps." + mapName);

	tileset = make_unique<Tileset>(mapData.GetVariableValue("tileset"));
	sizeInTiles.x = stoi(mapData.GetVariableValue("widthintiles"));
	sizeInTiles.y = stoi(mapData.GetVariableValue("heightintiles"));

	data.resize(sizeInTiles.x * sizeInTiles.y);

	string file = mapData.GetVariableValue("file");


	ifstream fstream(file);

	assert(fstream.is_open());

	size_t index{};
	string line{};
	while (getline(fstream, line))
	{
		stringstream sstream(line);

		string id{};
		while (getline(sstream, id, ','))
		{
			data[index] = stoi(id);
			index++;
		}	
	}

	worldSize = make_float2(tileset->tileSize * sizeInTiles);
	
	BoundingBox worldBox({ 0.0f, 0.0f }, worldSize);
	scene.camera->SetLockInsideBounds(worldBox);
	scene.quadTree->Resize(worldBox);
}

void TileMap::Draw(Surface* target, const Camera* camera) const
{
	const BoundingBox& viewport = camera->GetViewport();

	uint2 topLeftTile = WorldToTile(viewport.GetTopLeft());
	uint2 bottomRightTile = WorldToTile(viewport.GetBottomRight()) + uint2(1, 1);

	topLeftTile.x = max(topLeftTile.x, 0u);
	topLeftTile.y = max(topLeftTile.y, 0u);
	bottomRightTile.x = min(bottomRightTile.x, sizeInTiles.x);
	bottomRightTile.y = min(bottomRightTile.y, sizeInTiles.y);

	MySprite* sprite = tileset->sprite.get();

	int2 topLeftTileScreenPos = camera->WorldToVirtualScreen(TileToWorld(topLeftTile));
	int2 screenPos = topLeftTileScreenPos;

	for (uint y = topLeftTile.y; y < bottomRightTile.y; y++, screenPos.y += tileset->tileSize)
	{
		screenPos.x = topLeftTileScreenPos.x;

		for (uint x = topLeftTile.x; x < bottomRightTile.x; x++, screenPos.x += tileset->tileSize)
		{
			UINT8 id = data[x + y * sizeInTiles.x];
			sprite->SetFrame(id);
			sprite->Draw(target, screenPos);
		}
	}
}

uint2 TileMap::WorldToTile(float2 worldPos) const
{
	float2 f = worldPos / static_cast<float>(tileset->tileSize);
	return uint2(static_cast<uint>(f.x), static_cast<uint>(f.y));
}

float2 TileMap::TileToWorld(uint2 tilePos) const
{
	return make_float2(tilePos * TileSizeWorld());
}

uint TileMap::TileSizeWorld() const
{
	return tileset->tileSize;
}

uint TileMap::TileSizeScreen() const
{
	return tileset->tileSize;
}
