#include "stdafx.h"
#include "Tile.h"
#include "shader.h"
// TODO: сделать возможность запечь буфер в статический (сейчас они динамические), когда карта завершена (но оставить динамик для каких-нибудь лифтов)
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
//-----------------------------------------------------------------------------
void PartTile::Init()
{
	Close();

	createVertexBuffer();
	
	if (indexBuffer.id == 0 || numberIndexBuffer < 1)
	{
		const sg_buffer_desc indexBufferDesc = { .type = SG_BUFFERTYPE_INDEXBUFFER, .data = SG_RANGE(indexBuf) };
		indexBuffer = sg_make_buffer(&indexBufferDesc);
	}
	numberIndexBuffer++;
	m_bind.index_buffer = indexBuffer;

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
//-----------------------------------------------------------------------------
void PartTile::Close()
{
	if (m_bind.vertex_buffers[0].id == 0) return;

	sg_destroy_buffer(m_bind.vertex_buffers[0]);
	numberIndexBuffer--;
	if (numberIndexBuffer<1)
		sg_destroy_buffer(indexBuffer);

	sg_destroy_image(m_bind.fs_images[SLOT_diffuse_texture]);	
}
//-----------------------------------------------------------------------------
void PartTile::Update(bool staticBuf)
{
	if (staticBuf)
	{
		Init();
	}
	else
	{
		// дальний левый/дальний правый/ближний левый/ближний правый
		float top[4] = { 0.4, 0.3, 0.2, 0.1 };
		float bottom[4] = { 0.1, 0.2, 0.3, 0.4 };
		TileVertex vert[4];

		if (m_side == PartTileSide::Top)
		{
			for (int i = 0; i < 4; i++)
			{
				vert[i] = topVertices[i];
				vert[i].pos.Y -= top[i];
			}
		}
		else if (m_side == PartTileSide::Bottom)
		{
			for (int i = 0; i < 4; i++)
			{
				vert[i] = bottomVertices[i];
				vert[i].pos.Y += bottom[i];
			}
		}
		else if (m_side == PartTileSide::Forward)
		{
			for (int i = 0; i < 4; i++)
				vert[i] = forwardVertices[i];

			vert[0].pos.Y -= top[2];
			vert[1].pos.Y -= top[3];
			vert[0].texCoord.V += top[2];
			vert[1].texCoord.V += top[3];

			vert[2].pos.Y += bottom[2];
			vert[3].pos.Y += bottom[3];
			vert[2].texCoord.V -= bottom[2];
			vert[3].texCoord.V -= bottom[3];
		}
		else if (m_side == PartTileSide::Back)
		{
			for (int i = 0; i < 4; i++)
				vert[i] = backVertices[i];

			vert[0].pos.Y -= top[0];
			vert[1].pos.Y -= top[1];
			vert[0].texCoord.V += top[0];
			vert[1].texCoord.V += top[1];

			vert[2].pos.Y += bottom[0];
			vert[3].pos.Y += bottom[1];
			vert[2].texCoord.V -= bottom[0];
			vert[3].texCoord.V -= bottom[1];
		}
		else if (m_side == PartTileSide::Left)
		{
			for (int i = 0; i < 4; i++)
				vert[i] = leftVertices[i];

			vert[0].pos.Y -= top[0];
			vert[2].pos.Y -= top[2];
			vert[0].texCoord.V += top[0];
			vert[2].texCoord.V += top[2];

			vert[1].pos.Y += bottom[0];
			vert[3].pos.Y += bottom[2];
			vert[1].texCoord.V -= bottom[0];
			vert[3].texCoord.V -= bottom[2];
		}
		else if (m_side == PartTileSide::Right)
		{
			for (int i = 0; i < 4; i++)
				vert[i] = rightVertices[i];

			vert[0].pos.Y -= top[1];
			vert[2].pos.Y -= top[3];
			vert[0].texCoord.V += top[1];
			vert[2].texCoord.V += top[3];

			vert[1].pos.Y += bottom[1];
			vert[3].pos.Y += bottom[3];
			vert[1].texCoord.V -= bottom[1];
			vert[3].texCoord.V -= bottom[3];
		}

		sg_update_buffer(m_bind.vertex_buffers[0], SG_RANGE(vert));
	}
}
//-----------------------------------------------------------------------------
void PartTile::Draw()
{
	sg_apply_bindings(&m_bind);
	sg_draw(0, 6, 1);
}
//-----------------------------------------------------------------------------
void PartTile::createVertexBuffer()
{
	sg_buffer_desc vertexBufferDesc = { .size = sizeof(TileVertex)*4, .type = SG_BUFFERTYPE_VERTEXBUFFER, .usage = SG_USAGE_DYNAMIC};
	sg_buffer buffer = sg_make_buffer(&vertexBufferDesc);
	m_bind.vertex_buffers[0] = buffer;
	//planeBufferDesc.data = SG_RANGE(vert);
	Update();
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
void Tile::Update()
{
	m_top.Update();
	m_bottom.Update();
	m_forward.Update();
	m_back.Update();
	m_left.Update();
	m_right.Update();
}
//-----------------------------------------------------------------------------
void Tile::Draw()
{
	m_top.Draw();
	m_bottom.Draw();
	m_forward.Draw();
	m_back.Draw();
	m_left.Draw();
	m_right.Draw();
}
//-----------------------------------------------------------------------------