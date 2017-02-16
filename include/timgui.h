

#ifndef _TOWERENGINE_TIMGUI_H
#define _TOWERENGINE_TIMGUI_H

#ifdef TOWERENGINE_ENABLE_IMGUI

namespace tImGui
{
	void Init();
	void Shutdown();

	void BeginFrame(float delta_time, int width, int height, float width_scale = 1.0f, float height_scale = 1.0f);
}

#endif

#endif
