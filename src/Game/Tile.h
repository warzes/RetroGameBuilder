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

// дальний левый/дальний правый/ближний левый/ближний правый
struct HeightTile
{
	float& operator[](size_t i) { return (&bl)[i]; }
	const float operator[](size_t i) const { return (&bl)[i]; }

	float bl = 0.0f;
	float br = 0.0f;
	float fl = 0.0f;
	float fr = 0.0f;
};

// часть тайла
class PartTile
{
public:
	PartTile(PartTileSide side) : m_side(side) {}

	void Init();
	void Close();

	void UpdateGeometry(const HeightTile& heightTop, const HeightTile& heightBottom);

	void Draw();

private:
	void createVertexBuffer(const HeightTile& heightTop, const HeightTile& heightBottom);

	PartTileSide m_side;
	sg_bindings m_bind = {};
};

class Tile
{
public:
	void Init();
	void Close();

	void SetHeights(const HeightTile& heightTop, const HeightTile& heightBottom);

	void UpdateGeometry();
	void Draw();

private:
	PartTile m_top = PartTile(PartTileSide::Top);
	PartTile m_bottom = PartTile(PartTileSide::Bottom);
	PartTile m_forward = PartTile(PartTileSide::Forward);
	PartTile m_back = PartTile(PartTileSide::Back);
	PartTile m_left = PartTile(PartTileSide::Left);
	PartTile m_right = PartTile(PartTileSide::Right);

	HeightTile m_heightTop = {0.0f, 0.0f, 0.0f, 0.0f};
	HeightTile m_heightBottom = { 0.0f, 0.0f, 0.0f, 0.0f };
};