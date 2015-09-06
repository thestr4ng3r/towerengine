
#ifndef _REFRACTION_SHADER_H
#define _REFRACTION_SHADER_H

class tRefractionShader : public tFaceShader
{
	private:
		GLint modelview_projection_matrix_uniform;

		GLint transformation_uniform;

		GLint color_uniform;

		GLint color_tex_enabled_uniform;
		GLint color_tex_uniform;

		GLint normal_tex_uniform;

		GLint screen_tex_uniform;

	public:
		static const int screen_tex_unit = 1;

		void Init(void);

		void SetModelViewProjectionMatrix(float m[16]);
		void SetColor(tVector color);
		void SetColorTexture(bool enabled, GLuint tex = 0);
		void SetNormalTexture(GLuint tex);
		void SetScreenTexture(GLuint tex);
		void SetTransformation(const float m[16]);
};


#endif
