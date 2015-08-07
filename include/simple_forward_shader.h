
#ifndef _SIMPLE_FORWARD_SHADER_H
#define _SIMPLE_FORWARD_SHADER_H

class tSimpleForwardShader : public tFaceShader
{
	private:
		GLint modelview_projection_matrix_uniform;

		GLint transformation_uniform;

		GLint color_uniform;

		GLint tex_enabled_uniform;
		GLint tex_uniform;

	public:
		void Init(void);

		void SetModelViewProjectionMatrix(float m[16]);
		void SetColor(tVector color, float alpha);
		void SetTexture(bool enabled, GLuint tex = 0);
		void SetTransformation(const float m[16]);
};


#endif
