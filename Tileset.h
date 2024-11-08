#pragma once

class MySprite;

class Tileset
{
public:
	Tileset(const string& type);
	~Tileset();

	uint tileSize{};
	unique_ptr<MySprite> sprite{};
private:
	uint2 sizeInTiles{};
};

