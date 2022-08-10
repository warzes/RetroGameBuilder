#include "stdafx.h"
#include "World.h"
//-----------------------------------------------------------------------------
void World::Init()
{
	m_cameraMgr.Init();
}
//-----------------------------------------------------------------------------
void World::Update(float deltaTime)
{
	m_cameraMgr.Update(deltaTime);
}
//-----------------------------------------------------------------------------
void World::Input(const sapp_event* e, hmm_vec2 mouseOffset)
{
	m_cameraMgr.Input(e, mouseOffset);
}
//-----------------------------------------------------------------------------