
#ifndef _POINT_LIGHTING_SHADER_H
#define _POINT_LIGHTING_SHADER_H


class tPointLightingShader : public tScreenShader
{
	private:
		GLint cam_pos_uniform;

		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;
		GLint point_light_shadow_map_uniform;

		GLint position_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;

		int point_light_shadow_tex_unit;

	public:
		void Init(tGBuffer *gbuffer);

		void SetPointLight(tVector pos, tVector color, float dist, int shadow_enabled, GLuint shadow_map);
		void SetCameraPosition(tVector pos);

};


#endif
