#pragma once

enum class CameraType
{
	Orbital,
	Free
};

struct OrbitalCameraDesc
{
	hmm_vec3 target;
	hmm_vec3 up;
	float pitch;
	float heading;
	float distance;
	// camera limits
	float min_pitch;
	float max_pitch;
	float min_dist;
	float max_dist;
	// control options
	float rotate_speed;
	float zoom_speed;
};

class OrbitalCamera
{
public:
	void Set(const OrbitalCameraDesc& desc);

	void Input(const sapp_event* e, hmm_vec2 mouse_offset);

	hmm_mat4 GetViewMatrix() const { return HMM_LookAt(position, target, up); }
	hmm_vec3 GetPosition() const { return position; }

	// camera config
	hmm_vec3 target;
	hmm_vec3 up;
	hmm_vec2 polar;
	float distance;
	// camera limits
	float min_pitch;
	float max_pitch;
	float min_dist;
	float max_dist;
	// control options
	float rotate_speed;
	float zoom_speed;
	bool enable_rotate;
	// internal state
	hmm_vec3 position;

private:
	void updateVectors();
	void mouseMove(hmm_vec2 mouse_offset); // TODO: rename
	void zoom(float val);
};

struct FreeCameraDesc 
{
	hmm_vec3 position;
	hmm_vec3 world_up;
	float yaw;
	float pitch;
	float zoom;
	// limits
	float min_pitch;
	float max_pitch;
	float min_zoom;
	float max_zoom;
	// control options
	float movement_speed;
	float aim_speed;
	float zoom_speed;
};

class FreeCamera
{
public:
	void Set(const FreeCameraDesc& desc);
	void Update(float delta_time);
	void Input(const sapp_event* e, hmm_vec2 mouse_offset);

	hmm_mat4 GetViewMatrix() const 
	{
		const hmm_vec3 target = HMM_AddVec3(position, front);
		return HMM_LookAt(position, target, up);
	}
	hmm_vec3 GetPosition() const { return position; }

	// camera attributes
	hmm_vec3 position;
	hmm_vec3 world_up;
	float yaw;
	float pitch;
	float m_zoom;
	// limits
	float min_pitch;
	float max_pitch;
	float min_zoom;
	float max_zoom;
	// control options
	float movement_speed;
	float aim_speed;
	float zoom_speed;
	// control state
	bool enable_aim;
	bool move_forward;
	bool move_backward;
	bool move_left;
	bool move_right;
	// internal state
	hmm_vec3 front;
	hmm_vec3 up;
	hmm_vec3 right;

private:
	void updateVectors();
	void move(hmm_vec2 mouse_offset); // TODO: rename
	void zoom(float yoffset);
};

class CameraManager
{
public:
	void Init();

	void SetCameraType(CameraType type) 
	{
		if (type == m_cameraType) return;
		m_cameraType = type; 
		
	}
	CameraType GetCameraType() const { return m_cameraType; }

	hmm_mat4 GetCameraViewMatrix() const
	{
		if (m_cameraType == CameraType::Orbital) return m_orbitalCamera.GetViewMatrix();
		else if (m_cameraType == CameraType::Free) return m_freeCamera.GetViewMatrix();
		else {};
	}
	hmm_vec3 GetCameraPosition() const
	{
		if (m_cameraType == CameraType::Orbital) return m_orbitalCamera.GetPosition();
		else if (m_cameraType == CameraType::Free) return m_freeCamera.GetPosition();
		else {};
	}

	void Input(const sapp_event* e, hmm_vec2 mouse_offset)
	{
		if (m_cameraType == CameraType::Orbital) m_orbitalCamera.Input(e, mouse_offset);
		else if (m_cameraType == CameraType::Free) m_freeCamera.Input(e, mouse_offset);
		else {};


	}

	void Update(float delta_time)
	{
		if (m_cameraType == CameraType::Orbital);
		else if (m_cameraType == CameraType::Free) m_freeCamera.Update(delta_time);
		else {};
	}

private:
	CameraType m_cameraType = CameraType::Free;
	OrbitalCamera m_orbitalCamera;
	FreeCamera m_freeCamera;
};