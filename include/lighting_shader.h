
#ifndef _LIGHTING_SHADER_H
#define _LIGHTING_SHADER_H

#include <vector>
#include "material.h"

class tLightingShader : public tScreenShader
{
	private:
		GLint position_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;
		GLint self_illumination_tex_uniform;

		GLint ssao_enabled_uniform;
		GLint ssao_tex_uniform;

		GLint cam_pos_uniform;

		// ambient lighting
		GLint light_ambient_color_uniform;

		// point lighting
		/*GLint point_light_count_uniform;
		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;*/
		GLint point_light_shadow_map_uniform;

	public:
		static const int point_light_binding_point = 0;

		static const int max_point_lights_count = 32;

		void Init(tGBuffer *gbuffer);

		void SetAmbientLight(tVector color);
		void SetCameraPosition(tVector pos);
		void SetSSAO(bool enabled, GLuint64 tex_handle);
};



#endif
