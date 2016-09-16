
#ifndef _COLOR_FACE_SHADER_H
#define _COLOR_FACE_SHADER_H


class tColorShader : public tShader
{
	private:
		GLint modelview_projection_matrix_uniform;
		GLint transformation_uniform;

	public:
		static const GLint color_attribute = 1;

		tColorShader(void);

		void SetModelViewProjectionmatrix(const float m[16]);
		void SetTransformation(const float m[16]);
};


#endif
