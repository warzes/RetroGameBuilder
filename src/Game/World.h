#pragma once

#include "Camera.h"

class World
{
public:
	void Init();

	void SetCameraType(CameraType type) { m_cameraType = type; }
	CameraType GetCameraType() const { return m_cameraType; }
	//OrbitalCamera& GetOrbitalCamera() { return m_orbitalCamera; }
	//FreeCamera& GetFreeCamera() { return m_freeCamera; }
	hmm_mat4 GetCameraViewMatrix() const
	{
		if (m_cameraType == CameraType::Orbital) return m_orbitalCamera.GetViewMatrix();
		else if (m_cameraType == CameraType::Free) return m_freeCamera.GetViewMatrix();
		else {};
	}
	hmm_vec3 GetPosition() const
	{ 
		if (m_cameraType == CameraType::Orbital) return m_orbitalCamera.GetPosition();
		else if (m_cameraType == CameraType::Free) return m_freeCamera.GetPosition();
		else {};
	}

	void Input(const sapp_event* e, hmm_vec2 mouse_offset)
	{
		if (m_cameraType == CameraType::Orbital) return m_orbitalCamera.Input(e, mouse_offset);
		else if (m_cameraType == CameraType::Free) return m_freeCamera.Input(e, mouse_offset);
		else {};
	}

	void UpdateCamera(float delta_time)
	{
		if (m_cameraType == CameraType::Orbital);
		else if (m_cameraType == CameraType::Free) return m_freeCamera.Update(delta_time);
		else {};
	}

private:
	CameraType m_cameraType = CameraType::Orbital;
	OrbitalCamera m_orbitalCamera;
	FreeCamera m_freeCamera;
};