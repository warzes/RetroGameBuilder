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

	void Move(hmm_vec2 mouse_offset); // TODO: rename
	void Zoom(float yoffset);
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
};