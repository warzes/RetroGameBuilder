#pragma once

#include "Camera.h"
#include "TileMap.h"

class World
{
public:
	void Init();
	void Close();

	void Update(float deltaTime);
	void Draw();
	void Input(const sapp_event* e, hmm_vec2 mouseOffset);
	
	CameraManager& GetCameraManager() { return m_cameraMgr; }
	const CameraManager& GetCameraManager() const { return m_cameraMgr; }

private:
	CameraManager m_cameraMgr;
	TileMap m_tileMap;
};