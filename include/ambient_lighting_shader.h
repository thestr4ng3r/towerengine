
#ifndef _AMBIENT_LIGHTING_SHADER_H
#define _AMBIENT_LIGHTING_SHADER_H


class tAmbientLightingShader : public tScreenShader
{
	private:
		GLint depth_tex_uniform;
		GLint diffuse_tex_uniform;
		GLint self_illumination_tex_uniform;

		GLint light_ambient_color_uniform;

	public:
		void Init(tGBuffer *gbuffer);

		void SetAmbientLight(tVector color);
};



#endif
