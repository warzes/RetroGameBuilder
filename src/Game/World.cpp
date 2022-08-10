#include "stdafx.h"
#include "World.h"
#include "shader.h"
//-----------------------------------------------------------------------------
void World::Init()
{
	m_cameraMgr.Init();
	m_tileMap.Init();
}
//-----------------------------------------------------------------------------
void World::Close()
{
	m_tileMap.Close();
}
//-----------------------------------------------------------------------------
void World::Update(float deltaTime)
{
	m_cameraMgr.Update(deltaTime);
	m_tileMap.Update(deltaTime);
}
//-----------------------------------------------------------------------------
void World::Input(const sapp_event* e, hmm_vec2 mouseOffset)
{
	m_cameraMgr.Input(e, mouseOffset);
}
//-----------------------------------------------------------------------------
void World::Draw()
{
	m_tileMap.Draw(*this);
}
//-----------------------------------------------------------------------------