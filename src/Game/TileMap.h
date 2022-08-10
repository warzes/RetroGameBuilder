#pragma once

#include "Tile.h"

class World;

class TileMap
{
public:
	void Init();
	void Close();
	void Update(float deltaTime);
	void Draw(const World& world);

	Tile m_tile;
};