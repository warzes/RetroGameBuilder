#include "stdafx.h"
#include "World.h"
//-----------------------------------------------------------------------------
void World::Init()
{
	m_orbitalCamera.Set({
		.target = {0.0f, 0.0f, 0.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.pitch = 0.f,
		.heading = 0.f,
		.distance = 6.f,
		.min_pitch = -89.f,
		.max_pitch = 89.f,
		.min_dist = 1.f,
		.max_dist = 10.f,
		.rotate_speed = 1.f,
		.zoom_speed = .5f,
		});

	m_freeCamera.Set({
		.position = {0.0f, 0.0f, 6.0f},
		.world_up = {0.0f, 1.0f, 0.0f},
		.yaw = -90.f,
		.pitch = 0.f,
		.zoom = 45.f,
		.min_pitch = -89.f,
		.max_pitch = 89.f,
		.min_zoom = 1.f,
		.max_zoom = 45.f,
		.movement_speed = 0.005f,
		.aim_speed = 1.f,
		.zoom_speed = .1f,
		});
}
//-----------------------------------------------------------------------------