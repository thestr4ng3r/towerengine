
#ifndef _SSAO_LIGHTING_SHADER_H
#define _SSAO_LIGHTING_SHADER_H


class tSSAOLightingShader : public tScreenShader
{
	private:
		GLint depth_tex_uniform;
		GLint ssao_tex_uniform;

		int ssao_tex_unit;

	public:
		void Init(tGBuffer *gbuffer);

		void SetSSAOTexture(GLuint tex);
};


#endif
