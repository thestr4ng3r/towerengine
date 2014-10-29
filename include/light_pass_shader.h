
#ifndef _LIGHT_PASS_SHADER_H
#define _LIGHT_PASS_SHADER_H


class tLightPassShader : public tShader
{
	private:
		GLint cam_pos_uniform;

		GLint point_light_count_uniform;
		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;
		GLint point_light_shadow_map_uniform;

		GLint directional_light_count_uniform;
		GLint directional_light_dir_uniform;
		GLint directional_light_color_uniform;
		GLint directional_light_shadow_enabled_uniform;
		GLint directional_light_shadow_clip_uniform;
		GLint directional_light_shadow_tex_matrix_uniform;
		GLint directional_light_shadow_splits_count_uniform;
		GLint directional_light_shadow_splits_z_uniform;
		GLint directional_light_shadow_map_uniform;

		GLint light_ambient_color_uniform;

		GLint position_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;
		GLint specular_exponent_tex_uniform;

		GLint ssao_enabled_uniform;
		GLint ssao_tex_uniform;


	public:
		static const GLint max_point_lights = 8;
		static const GLint max_directional_lights = 8;
		static const GLint max_directional_light_splits = 8;

		static const unsigned int point_light_shadow_tex_first_unit = 6;
		static const unsigned int directional_light_shadow_tex_first_unit = point_light_shadow_tex_first_unit + max_point_lights;


		void Init(void);

		void SetGBuffer(tGBuffer *gbuffer);
		void SetSSAO(bool enabled, GLuint tex);
		void SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps);
		void SetDirectionalLights(int count, float *dir, float *color, int *shadow_enabled, GLuint *shadow_maps, float *shadow_clip, float *shadow_tex_matrix, float *shadow_splits_count, float *shadow_splits_z);
		void SetLightAmbientColor(tVector color);
		void SetCameraPosition(tVector pos);
};


#endif
