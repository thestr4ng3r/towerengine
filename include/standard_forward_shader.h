
#ifndef _STANDARD_FORWARD_SHADER_H
#define _STANDARD_FORWARD_SHADER_H

class tStandardForwardShader : public tFaceShader
{
	private:
		GLint transformation_uniform;

		GLint color_uniform;

		GLint tex_enabled_uniform;
		GLint tex_uniform;

	public:
		tStandardForwardShader(void);

		void SetTransformation(const float m[16]);
};


#endif
