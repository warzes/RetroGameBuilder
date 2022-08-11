#pragma once

struct TileVertex
{
	constexpr TileVertex() = default;
	constexpr TileVertex(hmm_vec3 p, hmm_vec2 tc) : pos(p), texCoord(tc) {}

	hmm_vec3 pos = {};
	hmm_vec2 texCoord = {};
};

enum class PartTileSide
{
	Top,
	Bottom,
	Forward,
	Back,
	Left,
	Right
};

struct Rect
{
	float r1, r2, r3, r4;
};

// часть тайла
class PartTile
{
public:
	PartTile(PartTileSide side) : m_side(side) {}

	void Init();
	void Close();

	void Update(bool staticBuf = false);

	void Draw();

private:
	void createVertexBuffer();

	PartTileSide m_side;
	sg_bindings m_bind = {};
};

class Tile
{
public:
	void Init();
	void Close();

	void Update();
	void Draw();

private:
	PartTile m_top = PartTile(PartTileSide::Top);
	PartTile m_bottom = PartTile(PartTileSide::Bottom);
	PartTile m_forward = PartTile(PartTileSide::Forward);
	PartTile m_back = PartTile(PartTileSide::Back);
	PartTile m_left = PartTile(PartTileSide::Left);
	PartTile m_right = PartTile(PartTileSide::Right);
};