#include "precomp.h"
#include "Tileset.h"
#include "PlayerData.h"
#include "MySprite.h"

Tileset::Tileset(const string& type)
{
	PlayerData tilesetData("tilesets." + type);

	sizeInTiles.x = stoi(tilesetData.GetVariableValue("widthintiles"));
	sizeInTiles.y = stoi(tilesetData.GetVariableValue("heightintiles"));

	string file = tilesetData.GetVariableValue("file");
	shared_ptr surface = make_shared<Surface>(file.c_str());
	tileSize = surface->width / sizeInTiles.x;
	sprite = make_unique<MySprite>(move(surface), sizeInTiles.x, sizeInTiles.y);
}

Tileset::~Tileset()
{

}