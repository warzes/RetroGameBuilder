#pragma once

#include "Camera.h"

class World
{
public:
	void Init();

	void Update(float deltaTime);
	void Input(const sapp_event* e, hmm_vec2 mouseOffset);
	
	CameraManager& GetCameraManager() { return m_cameraMgr; }
	const CameraManager& GetCameraManager() const { return m_cameraMgr; }

private:
	CameraManager m_cameraMgr;
};