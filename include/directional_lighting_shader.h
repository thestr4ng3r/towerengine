
#ifndef _DIRECTIONAL_LIGHT_SHADER_H
#define _DIRECTIONAL_LIGHT_SHADER_H


class tDirectionalLightingShader : public tScreenShader
{
	private:
		GLint cam_pos_uniform;

		GLint directional_light_dir_uniform;
		GLint directional_light_color_uniform;
		GLint directional_light_shadow_enabled_uniform;
		GLint directional_light_shadow_clip_uniform;
		GLint directional_light_shadow_tex_matrix_uniform;
		GLint directional_light_shadow_splits_count_uniform;
		GLint directional_light_shadow_splits_z_uniform;
		GLint directional_light_shadow_map_uniform;

		GLint position_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;

		int directional_light_shadow_tex_unit;

	public:
		static const GLint max_directional_light_splits = 8;


		void Init(tGBuffer *gbuffer);

		void SetDirectionalLight(tVector dir, tVector color, int shadow_enabled, GLuint shadow_map, tVector2 shadow_clip, float *shadow_tex_matrix, int shadow_splits_count, float *shadow_splits_z);
		void SetCameraPosition(tVector pos);

};


#endif
