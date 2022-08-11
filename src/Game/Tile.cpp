#include "stdafx.h"
#include "Tile.h"
#include "shader.h"
// TODO: сделать возможность запечь буфер в статический (сейчас они динамические), когда карта завершена (но оставить динамик для каких-нибудь лифтов)
// TODO: возможно вместо двух треугольник делать 4 треугольника на сторону куба/тайла
//-----------------------------------------------------------------------------
void failCallback();
uint8_t file_buffer[2 * 1024 * 1024];
//-----------------------------------------------------------------------------
constexpr TileVertex topVertices[] =
{
	{{ -0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{  0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, 0.5f,  0.5f}, {0.0f, 1.0f}},
	{{  0.5f, 0.5f,  0.5f}, {1.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr TileVertex bottomVertices[] =
{
	{{ -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{  0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}},
	{{  0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr TileVertex forwardVertices[] =
{
	{{ -0.5f,  0.5f, 0.5f}, {0.0f, 0.0f}},
	{{  0.5f,  0.5f, 0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
	{{  0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr TileVertex backVertices[] =
{
	{{ -0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}},
	{{  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}},
	{{ -0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{  0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr TileVertex leftVertices[] =
{
	{{ -0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{ -0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
	{{ -0.5f, 0.5f,  0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr TileVertex rightVertices[] =
{
	{{ 0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}},
};
//-----------------------------------------------------------------------------
constexpr uint16_t indexBuf[] =
{
	0, 1, 2,
	1, 3, 2
};
//-----------------------------------------------------------------------------
// index buffer один на всех
static sg_buffer indexBuffer = { 0 };
static int32_t numberIndexBuffer = 0;

// TODO: пока одна текстура
static sg_image img_id = { 0 };
static int32_t numberImg = 0;

static sg_bindings bindRes = {};
//-----------------------------------------------------------------------------
void PartTile::Init()
{
	Close();

	createVertexBuffer({ 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f });
	
	if (indexBuffer.id == 0 || numberIndexBuffer < 1)
	{
		const sg_buffer_desc indexBufferDesc = { .type = SG_BUFFERTYPE_INDEXBUFFER, .data = SG_RANGE(indexBuf) };
		indexBuffer = sg_make_buffer(&indexBufferDesc);
	}
	numberIndexBuffer++;

	if (img_id.id == 0 || numberImg < 1)
	{
		img_id = sg_alloc_image();
		lopgl_image_request_t imageDesc1 = {
			.path = "../CoreData/textures/1mx1m.png",
			.img_id = img_id,
			.buffer_ptr = file_buffer,
			.buffer_size = sizeof(file_buffer),
			.fail_callback = failCallback
		};
		lopgl_load_image(&imageDesc1);
	}
	numberImg++;
}
//-----------------------------------------------------------------------------
void PartTile::Close()
{
	if (m_buffer.id == 0) return;

	sg_destroy_buffer(m_buffer);
	
	numberIndexBuffer--;
	if (numberIndexBuffer<1)
		sg_destroy_buffer(indexBuffer);

	numberImg--;
	if (numberImg < 1)
		sg_destroy_image(img_id);
}
//-----------------------------------------------------------------------------
void PartTile::UpdateGeometry(const HeightTile& heightTop, const HeightTile& heightBottom)
{
	TileVertex vert[4];

	if (m_side == PartTileSide::Top)
	{
		for (int i = 0; i < 4; i++)
		{
			vert[i] = topVertices[i];
			vert[i].pos.Y -= heightTop[i];
		}
	}
	else if (m_side == PartTileSide::Bottom)
	{
		for (int i = 0; i < 4; i++)
		{
			vert[i] = bottomVertices[i];
			vert[i].pos.Y += heightBottom[i];
		}
	}
	else if (m_side == PartTileSide::Forward)
	{
		for (int i = 0; i < 4; i++)
			vert[i] = forwardVertices[i];

		vert[0].pos.Y -= heightTop[2];
		vert[1].pos.Y -= heightTop[3];
		vert[0].texCoord.V += heightTop[2];
		vert[1].texCoord.V += heightTop[3];

		vert[2].pos.Y += heightBottom[2];
		vert[3].pos.Y += heightBottom[3];
		vert[2].texCoord.V -= heightBottom[2];
		vert[3].texCoord.V -= heightBottom[3];
	}
	else if (m_side == PartTileSide::Back)
	{
		for (int i = 0; i < 4; i++)
			vert[i] = backVertices[i];

		vert[0].pos.Y -= heightTop[0];
		vert[1].pos.Y -= heightTop[1];
		vert[0].texCoord.V += heightTop[0];
		vert[1].texCoord.V += heightTop[1];

		vert[2].pos.Y += heightBottom[0];
		vert[3].pos.Y += heightBottom[1];
		vert[2].texCoord.V -= heightBottom[0];
		vert[3].texCoord.V -= heightBottom[1];
	}
	else if (m_side == PartTileSide::Left)
	{
		for (int i = 0; i < 4; i++)
			vert[i] = leftVertices[i];

		vert[0].pos.Y -= heightTop[0];
		vert[2].pos.Y -= heightTop[2];
		vert[0].texCoord.V += heightTop[0];
		vert[2].texCoord.V += heightTop[2];

		vert[1].pos.Y += heightBottom[0];
		vert[3].pos.Y += heightBottom[2];
		vert[1].texCoord.V -= heightBottom[0];
		vert[3].texCoord.V -= heightBottom[2];
	}
	else if (m_side == PartTileSide::Right)
	{
		for (int i = 0; i < 4; i++)
			vert[i] = rightVertices[i];

		vert[0].pos.Y -= heightTop[1];
		vert[2].pos.Y -= heightTop[3];
		vert[0].texCoord.V += heightTop[1];
		vert[2].texCoord.V += heightTop[3];

		vert[1].pos.Y += heightBottom[1];
		vert[3].pos.Y += heightBottom[3];
		vert[1].texCoord.V -= heightBottom[1];
		vert[3].texCoord.V -= heightBottom[3];
	}

	sg_update_buffer(m_buffer, SG_RANGE(vert));
}
//-----------------------------------------------------------------------------
void PartTile::Draw()
{
	bindRes.vertex_buffers[0] = m_buffer;
	bindRes.index_buffer = indexBuffer;
	bindRes.fs_images[SLOT_diffuse_texture] = img_id;

	sg_apply_bindings(&bindRes);
	sg_draw(0, 6, 1);
}
//-----------------------------------------------------------------------------
void PartTile::createVertexBuffer(const HeightTile& heightTop, const HeightTile& heightBottom)
{
	sg_buffer_desc vertexBufferDesc = { .size = sizeof(TileVertex)*4, .type = SG_BUFFERTYPE_VERTEXBUFFER, .usage = SG_USAGE_DYNAMIC};
	m_buffer = sg_make_buffer(&vertexBufferDesc);
	UpdateGeometry(heightTop, heightBottom);
}
//-----------------------------------------------------------------------------
void Tile::Init()
{
	m_top.Init();
	m_bottom.Init();
	m_forward.Init();
	m_back.Init();
	m_left.Init();
	m_right.Init();
}
//-----------------------------------------------------------------------------
void Tile::Close()
{
	m_top.Close();
	m_bottom.Close();
	m_forward.Close();
	m_back.Close();
	m_left.Close();
	m_right.Close();
}
//-----------------------------------------------------------------------------
void Tile::SetHeights(const HeightTile& heightTop, const HeightTile& heightBottom)
{
	m_top.UpdateGeometry(heightTop, heightBottom);
	m_bottom.UpdateGeometry(heightTop, heightBottom);
	m_forward.UpdateGeometry(heightTop, heightBottom);
	m_back.UpdateGeometry(heightTop, heightBottom);
	m_left.UpdateGeometry(heightTop, heightBottom);
	m_right.UpdateGeometry(heightTop, heightBottom);
}
//-----------------------------------------------------------------------------
void Tile::Draw()
{
	if (m_topVisible)     m_top.Draw();
	if (m_bottomVisible)  m_bottom.Draw();
	if (m_forwardVisible) m_forward.Draw();
	if (m_backVisible)    m_back.Draw();
	if (m_leftVisible)    m_left.Draw();
	if (m_rightVisible)   m_right.Draw();
}
//-----------------------------------------------------------------------------
void Tile::VisibleTop(bool visible)
{
	m_topVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleBottom(bool visible)
{
	m_bottomVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleForward(bool visible)
{
	m_forwardVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleBack(bool visible)
{
	m_backVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleLeft(bool visible)
{
	m_leftVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleRight(bool visible)
{
	m_rightVisible = visible;
}
//-----------------------------------------------------------------------------
void Tile::VisibleFull(bool visible)
{
	m_topVisible = visible;
	m_bottomVisible = visible;
	m_forwardVisible = visible;
	m_backVisible = visible;
	m_leftVisible = visible;
	m_rightVisible = visible;
}
//-----------------------------------------------------------------------------