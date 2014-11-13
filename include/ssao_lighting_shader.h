
#ifndef _SSAO_LIGHTING_SHADER_H
#define _SSAO_LIGHTING_SHADER_H


class tSSAOLightingShader : public tShader
{
	private:
		GLint ssao_tex_uniform;
		GLint diffuse_tex_uniform;

	public:
		void Init(void);
		void SetSSAOTexture(GLuint tex);
		void SetGBuffer(tGBuffer *gbuffer);
};


#endif
