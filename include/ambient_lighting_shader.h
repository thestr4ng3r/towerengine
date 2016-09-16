
#ifndef _AMBIENT_LIGHTING_SHADER_H
#define _AMBIENT_LIGHTING_SHADER_H


class tAmbientLightingShader : public tScreenShader, public tReflectingShader
{
	private:
		GLint depth_tex_uniform;
		GLint base_color_tex_uniform;
		GLint normal_tex_uniform;
		GLint metal_rough_reflect_tex_uniform;
		GLint emission_tex_uniform;

		GLint ssao_tex_uniform;

		GLint light_ambient_color_uniform;

		GLint cam_pos_uniform;

		int ssao_tex_unit;

		GLint reflection_tex1_uniform;
		GLint reflection_tex2_uniform;
		GLint reflection_tex_blend_uniform;

		int reflection_tex1_unit;
		int reflection_tex2_unit;

	public:
		tAmbientLightingShader(tGBuffer *gbuffer, bool ambient_ssao);

		void SetAmbientLight(tVector color);
		void SetSSAOTexture(GLuint tex);
		void SetCameraPosition(tVector pos);

		void SetReflectionTextures(GLuint tex1, GLuint tex2, float blend);
};



#endif
