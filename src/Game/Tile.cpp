#include "stdafx.h"
#include "Tile.h"
#include "shader.h"

void failCallback();
uint8_t file_buffer[2 * 1024 * 1024];

constexpr TileVertex topVertices[] =
{
	{{ -0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{  0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, 0.5f,  0.5f}, {0.0f, 1.0f}},
	{{  0.5f, 0.5f,  0.5f}, {1.0f, 1.0f}},
};

constexpr TileVertex bottomVertices[] =
{
	{{ -0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{  0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}},
	{{  0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}},
};

constexpr TileVertex forwardVertices[] =
{
	{{ -0.5f,  0.5f, 0.5f}, {0.0f, 0.0f}},
	{{  0.5f,  0.5f, 0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
	{{  0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
};

constexpr TileVertex backVertices[] =
{
	{{ -0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}},
	{{  0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}},
	{{ -0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{  0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
};

constexpr TileVertex leftVertices[] =
{
	{{ -0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{ -0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
	{{ -0.5f, 0.5f,  0.5f}, {1.0f, 0.0f}},
	{{ -0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}},
};

constexpr TileVertex rightVertices[] =
{
	{{ 0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
	{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}},
};

constexpr uint16_t indexBuf[] =
{
	0, 1, 2,
	1, 3, 2
};
// TODO: один индексный буффер 

void PartTile::Init()
{
	float top[4] = {0.4, 0.3, 0.2, 0.1};
	float bottom[4] = {0.0, 0.0, 0.0, 0.0};

	sg_buffer_desc planeBufferDesc = { .type = SG_BUFFERTYPE_VERTEXBUFFER };

	TileVertex vert[4];
	if (m_side == PartTileSide::Top)
	{		
		for (int i = 0; i < 4; i++)
		{
			vert[i] = topVertices[i];
			vert[i].pos.Y -= top[i];
		}
	}
	planeBufferDesc.data = SG_RANGE(vert);


	if (m_side == PartTileSide::Bottom)
		planeBufferDesc.data = SG_RANGE(bottomVertices);
	else if (m_side == PartTileSide::Forward)
		planeBufferDesc.data = SG_RANGE(forwardVertices);
	else if (m_side == PartTileSide::Back)
		planeBufferDesc.data = SG_RANGE(backVertices);
	else if (m_side == PartTileSide::Left)
		planeBufferDesc.data = SG_RANGE(leftVertices);
	else if (m_side == PartTileSide::Right)
		planeBufferDesc.data = SG_RANGE(rightVertices);

	sg_buffer plane_buffer = sg_make_buffer(&planeBufferDesc);
	m_bind.vertex_buffers[0] = plane_buffer;

	const sg_buffer_desc planeIndexBufferDesc = { .type = SG_BUFFERTYPE_INDEXBUFFER, .data = SG_RANGE(indexBuf) };
	sg_buffer plane_indexbuffer = sg_make_buffer(&planeIndexBufferDesc);
	m_bind.index_buffer = plane_indexbuffer;

	sg_image metal_img_id = sg_alloc_image();
	m_bind.fs_images[SLOT_diffuse_texture] = metal_img_id;

	lopgl_image_request_t imageDesc1 = {
		.path = "../CoreData/textures/1mx1m.png",
		.img_id = metal_img_id,
		.buffer_ptr = file_buffer,
		.buffer_size = sizeof(file_buffer),
		.fail_callback = failCallback
	};
	lopgl_load_image(&imageDesc1);
}

void PartTile::Close()
{
}

void PartTile::Draw()
{
	sg_apply_bindings(&m_bind);
	sg_draw(0, 6, 1);
}

void Tile::Init()
{
	m_top.Init();
	m_bottom.Init();
	m_forward.Init();
	m_back.Init();
	m_left.Init();
	m_right.Init();
}

void Tile::Close()
{
	m_top.Close();
	m_bottom.Close();
	m_forward.Close();
	m_back.Close();
	m_left.Close();
	m_right.Close();
}

void Tile::Draw()
{
	m_top.Draw();
	m_bottom.Draw();
	m_forward.Draw();
	m_back.Draw();
	m_left.Draw();
	m_right.Draw();
}