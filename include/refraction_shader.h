
#ifndef _REFRACTION_SHADER_H
#define _REFRACTION_SHADER_H

class tRefractionShader : public tFaceShader
{
	private:
		GLint transformation_uniform;

		GLint color_uniform;
		GLint edge_color_uniform;

		GLint color_tex_enabled_uniform;
		GLint color_tex_uniform;

		GLint normal_tex_enabled_uniform;
		GLint normal_tex_uniform;

		GLint cam_pos_uniform;

		GLint screen_tex_uniform;

	public:
		static const int screen_tex_unit = 1;

		tRefractionShader(void);

		void SetColor(tVector color, tVector edge, float edge_alpha);
		void SetColorTexture(bool enabled, GLuint tex = 0);
		void SetNormalTexture(GLuint tex = 0);
		void SetScreenTexture(GLuint tex);
		void SetTransformation(const float m[16]);
		void SetCameraPosition(tVector pos);
};


#endif
