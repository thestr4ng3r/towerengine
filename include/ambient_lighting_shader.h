
#ifndef _AMBIENT_LIGHTING_SHADER_H
#define _AMBIENT_LIGHTING_SHADER_H


class tAmbientLightingShader : public tLightingShader
{
	private:
		GLint diffuse_tex_uniform;

		GLint light_ambient_color_uniform;

	public:
		void Init(void);

		void SetGBuffer(tGBuffer *gbuffer);
		void SetAmbientLight(tVector color);
};



#endif
