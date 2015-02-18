
#ifndef _SSAO_LIGHTING_SHADER_H
#define _SSAO_LIGHTING_SHADER_H


class tSSAOLightingShader : public tLightingShader
{
	private:
		GLint ssao_tex_uniform;
		GLint diffuse_tex_uniform;

		GLint light_ambient_color_uniform;

	public:
		void Init(void);

		void SetSSAOTexture(GLuint tex);
		void SetGBuffer(tGBuffer *gbuffer);
		void SetAmbientLight(tVector color);
};


#endif
