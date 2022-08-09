#include "stdafx.h"
#include "Camera.h"
//-----------------------------------------------------------------------------
hmm_vec2 last_touch[SAPP_MAX_TOUCHPOINTS] = { {0} };
//-----------------------------------------------------------------------------
void OrbitalCamera::Set(const OrbitalCameraDesc& desc)
{
	// camera attributes
	target = desc.target;
	up = desc.up;
	polar = HMM_Vec2(desc.pitch, desc.heading);
	distance = desc.distance;
	// limits
	min_pitch = desc.min_pitch;
	max_pitch = desc.max_pitch;
	min_dist = desc.min_dist;
	max_dist = desc.max_dist;
	// control options
	rotate_speed = desc.rotate_speed;
	zoom_speed = desc.zoom_speed;
	// control state
	enable_rotate = false;

	updateVectors();
}
void OrbitalCamera::Input(const sapp_event* e, hmm_vec2 mouse_offset)
{
	if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
			enable_rotate = true;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_UP) {
		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
			enable_rotate = false;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
		if (enable_rotate) {
			mouseMove(mouse_offset);
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_SCROLL) {
		zoom(e->scroll_y);
	}
	else if (e->type == SAPP_EVENTTYPE_TOUCHES_BEGAN) {
		for (int i = 0; i < e->num_touches; ++i) {
			const sapp_touchpoint* touch = &e->touches[i];
			last_touch[touch->identifier].X = touch->pos_x;
			last_touch[touch->identifier].Y = touch->pos_y;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_TOUCHES_MOVED) {
		if (e->num_touches == 1) {
			const sapp_touchpoint* touch = &e->touches[0];
			hmm_vec2* lastTouch = &last_touch[touch->identifier];

			hmm_vec2 mouse_offset = HMM_Vec2(0.0f, 0.0f);

			mouse_offset.X = touch->pos_x - lastTouch->X;
			mouse_offset.Y = lastTouch->Y - touch->pos_y;

			// reduce speed of touch controls
			mouse_offset.X *= 0.3;
			mouse_offset.Y *= 0.3;

			mouseMove(mouse_offset);
		}
		else if (e->num_touches == 2) {
			const sapp_touchpoint* touch0 = &e->touches[0];
			const sapp_touchpoint* touch1 = &e->touches[1];

			const hmm_vec2 v0 = HMM_Vec2(touch0->pos_x, touch0->pos_y);
			const hmm_vec2 v1 = HMM_Vec2(touch1->pos_x, touch1->pos_y);

			const hmm_vec2* prev_v0 = &last_touch[touch0->identifier];
			const hmm_vec2* prev_v1 = &last_touch[touch1->identifier];

			const float length0 = HMM_LengthVec2(HMM_SubtractVec2(v1, v0));
			const float length1 = HMM_LengthVec2(HMM_SubtractVec2(*prev_v1, *prev_v0));

			float diff = length0 - length1;
			// reduce speed of touch controls
			diff *= 0.1;

			zoom(diff);
		}

		// update all touch coords
		for (int i = 0; i < e->num_touches; ++i) {
			const sapp_touchpoint* touch = &e->touches[i];
			last_touch[touch->identifier].X = touch->pos_x;
			last_touch[touch->identifier].Y = touch->pos_y;
		}
	}

	updateVectors();
}
//-----------------------------------------------------------------------------
void OrbitalCamera::updateVectors()
{
	const float cos_p = cosf(HMM_ToRadians(polar.X));
	const float sin_p = sinf(HMM_ToRadians(polar.X));
	const float cos_h = cosf(HMM_ToRadians(polar.Y));
	const float sin_h = sinf(HMM_ToRadians(polar.Y));
	position = HMM_Vec3(
		distance *  cos_p * sin_h,
		distance * -sin_p,
		distance *  cos_p * cos_h
	);
}
//-----------------------------------------------------------------------------
void OrbitalCamera::mouseMove(hmm_vec2 mouse_offset)
{
	polar.Y -= mouse_offset.X * rotate_speed;
	const float pitch = polar.X + mouse_offset.Y * rotate_speed;
	polar.X = HMM_Clamp(min_pitch, pitch, max_pitch);
}
//-----------------------------------------------------------------------------
void OrbitalCamera::zoom(float val)
{
	const float new_dist = distance - val * zoom_speed;
	distance = HMM_Clamp(new_dist, min_dist, max_dist);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void FreeCamera::Set(const FreeCameraDesc& desc)
{
	// camera attributes
	position = desc.position;
	world_up = desc.world_up;
	yaw = desc.yaw;
	pitch = desc.pitch;
	zoom = desc.zoom;
	// limits
	min_pitch = desc.min_pitch;
	max_pitch = desc.max_pitch;
	min_zoom = desc.min_zoom;
	max_zoom = desc.max_zoom;
	// control options
	movement_speed = desc.movement_speed;
	aim_speed = desc.aim_speed;
	zoom_speed = desc.zoom_speed;
	// control state
	enable_aim = false;
	move_forward = false;
	move_backward = false;
	move_left = false;
	move_right = false;

	updateVectors();
}
//-----------------------------------------------------------------------------
void FreeCamera::Update(float delta_time)
{
	const float velocity = movement_speed * delta_time;
	if (move_forward) {
		const hmm_vec3 offset = HMM_MultiplyVec3f(front, velocity);
		position = HMM_AddVec3(position, offset);
	}
	if (move_backward) {
		const hmm_vec3 offset = HMM_MultiplyVec3f(front, velocity);
		position = HMM_SubtractVec3(position, offset);
	}
	if (move_left) {
		const hmm_vec3 offset = HMM_MultiplyVec3f(right, velocity);
		position = HMM_SubtractVec3(position, offset);
	}
	if (move_right) {
		const hmm_vec3 offset = HMM_MultiplyVec3f(right, velocity);
		position = HMM_AddVec3(position, offset);
	}
}
//-----------------------------------------------------------------------------
void FreeCamera::Move(hmm_vec2 mouse_offset)
{
	yaw += mouse_offset.X * aim_speed;
	pitch += mouse_offset.Y * aim_speed;

	pitch = HMM_Clamp(min_pitch, pitch, max_pitch);

	updateVectors();
}
//-----------------------------------------------------------------------------
void FreeCamera::Zoom(float yoffset)
{
	zoom -= yoffset * zoom_speed;
	zoom = HMM_Clamp(min_zoom, zoom, max_zoom);
}
//-----------------------------------------------------------------------------
void FreeCamera::Input(const sapp_event* e, hmm_vec2 mouse_offset)
{
	if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
		if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP) {
			move_forward = true;
		}
		else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN) {
			move_backward = true;
		}
		else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT) {
			move_left = true;
		}
		else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT) {
			move_right = true;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_KEY_UP) {
		if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP) {
			move_forward = false;
		}
		else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN) {
			move_backward = false;
		}
		else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT) {
			move_left = false;
		}
		else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT) {
			move_right = false;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
			enable_aim = true;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_UP) {
		if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
			enable_aim = false;
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
		if (enable_aim) {
			Move(mouse_offset);
		}
	}
	else if (e->type == SAPP_EVENTTYPE_MOUSE_SCROLL) {
		Zoom(e->scroll_y);
	}
}
//-----------------------------------------------------------------------------
void FreeCamera::updateVectors()
{
	// Calculate the new Front vector
	const hmm_vec3 vfront =
	{
		cosf(HMM_ToRadians(yaw)) * cosf(HMM_ToRadians(pitch)),
		sinf(HMM_ToRadians(pitch)),
		sinf(HMM_ToRadians(yaw)) * cosf(HMM_ToRadians(pitch))
	};
	front = HMM_NormalizeVec3(vfront);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	right = HMM_NormalizeVec3(HMM_Cross(front, world_up));
	up = HMM_NormalizeVec3(HMM_Cross(right, front));
}
//-----------------------------------------------------------------------------