#pragma once
#include "util.h"
#include <algorithm>

class Camera
{
public:
	static Camera &getInstance()
	{
		static Camera instance;
		return instance;
	}

	static Vector2f getPos() { return pos_; }
	static void setPos(Vector2f pos)
	{
		if (pos.x > GRID_SIZE / 2)
			pos.x = GRID_SIZE / 2;
		else if (pos.x < -GRID_SIZE / 2)
			pos.x = -GRID_SIZE / 2;
		if (pos.y > GRID_SIZE / 2)
			pos.y = GRID_SIZE / 2;
		else if (pos.y < -GRID_SIZE / 2)
			pos.y = -GRID_SIZE / 2;

		pos_ = pos;
	}

	static float getZoom() { return zoom_; }
	static void setZoom(const float zoom) { zoom_ = std::clamp(zoom, MIN_CAMERA_ZOOM, MAX_CAMERA_ZOOM); }
	static void zoomAtPos(Vector2f pos, float scale_factor)
	{
		float prev_zoom = zoom_;
		setZoom(getZoom() * scale_factor);

		Vector2f camera_delta = pos * (getZoom() - prev_zoom) / (getZoom() * prev_zoom);

		setPos(getPos() + camera_delta);
	}

	static Vector2f screenToWorld(const Vector2f screenPos)
	{
		return (screenPos) / zoom_ + pos_;
	}

	static Vector2f worldToScreen(const Vector2f worldPos)
	{
		return (worldPos - pos_) * zoom_;
	}

	static SDL_FRect resizeFRect(SDL_FRect rect)
	{
		Vector2f pos = worldToScreen({rect.x, rect.y});
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w *= zoom_;
		rect.h *= zoom_;

		return rect;
	}

private:
	Camera() = default;
	Camera(const Camera &) = delete;
	Camera &operator=(const Camera &) = delete;

	static Vector2f pos_;
	static Vector2f screen_;
	static float zoom_;
};