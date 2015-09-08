
#ifndef _LIGHTING_SHADER_H
#define _LIGHTING_SHADER_H

#include <vector>
#include "material.h"

class tLightingShader : public tScreenShader
{
	private:
		GLint position_tex_uniform;
		GLint normal_tex_uniform;
		GLint tang_tex_uniform;
		GLint uv_tex_uniform;
		GLint material_tex_uniform;

		GLint cam_pos_uniform;

		// material
		GLint diffuse_tex_uniform;

		// ambient lighting
		GLint light_ambient_color_uniform;

		// point lighting
		GLint point_light_count_uniform;
		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;
		GLint point_light_shadow_map_uniform;

	public:
		void Init(tGBuffer *gbuffer);

		void SetAmbientLight(tVector color);

		void SetCameraPosition(tVector pos);

		void SetMaterials(std::vector<tDefaultMaterial *> &materials);

		void SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint64 *shadow_map_handles);
};



#endif
