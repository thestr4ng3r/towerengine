
#ifndef _POINT_LIGHTING_SHADER_H
#define _POINT_LIGHTING_SHADER_H


class tPointLightingShader : public tScreenShader
{
	private:
		int lights_count;

		GLint cam_pos_uniform;

		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;
		GLint point_light_shadow_map_uniform;

		GLint depth_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;

		int *point_light_shadow_tex_unit;

	public:
		tPointLightingShader(int lights_count);
		~tPointLightingShader(void);

		void Init(tGBuffer *gbuffer);

		/**
		 * set all point light uniforms
		 * @param pos length must be 3 * lights_count
		 * @param color length must be 3 * lights_count
		 * @param dist length must be lights_count
		 * @param shadow_enabled length must be lights_count
		 * @param shadow_map length must be lights_count
		*/
		void SetPointLights(float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_map);

		void SetCameraPosition(tVector pos);

		int GetLightsCount(void)	{ return lights_count; }

};


#endif
