#pragma once

#include "Tile.h"

class World;

// мир не состоит из сплошных тайлов, есть много пустот, и тайлов реально может быть меньше 30%, поэтому чтобы не занимать память, сделано так
struct TilePtr
{
	~TilePtr() { delete tile; }
	Tile* tile = nullptr;
};

constexpr int SizeMap = 20;

struct TileLayout
{
	void Init()
	{
		for (int x = 0; x < SizeMap; x++)
		{
			for (int y = 0; y < SizeMap; y++)
			{
				tiles[x][y].Init();
			}
		}
	}

	void Close()
	{
		for (int x = 0; x < SizeMap; x++)
		{
			for (int y = 0; y < SizeMap; y++)
			{
				tiles[x][y].Close();
			}
		}
	}


	Tile tiles[SizeMap][SizeMap];
};

class TileMap
{
public:
	void Init();
	void Close();
	void Update(float deltaTime);
	void Draw(const World& world);

	//Tile m_tile;
	TileLayout* m_tiles = nullptr;
};