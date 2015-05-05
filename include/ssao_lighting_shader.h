
#ifndef _SSAO_LIGHTING_SHADER_H
#define _SSAO_LIGHTING_SHADER_H


class tSSAOLightingShader : public tLightingShader
{
	private:
		GLint ssao_tex_uniform;
		GLint diffuse_tex_uniform;

		GLint light_ambient_color_uniform;

		int ssao_tex_unit;

	public:
		void Init(tGBuffer *gbuffer);

		void SetSSAOTexture(GLuint tex);
		void SetAmbientLight(tVector color);
};


#endif
