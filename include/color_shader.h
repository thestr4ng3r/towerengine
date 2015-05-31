
#ifndef _COLOR_FACE_SHADER_H
#define _COLOR_FACE_SHADER_H


class tColorShader : public tFaceShader
{
	private:
		GLint transformation_uniform;

		GLint diffuse_color_uniform;

	public:
		void Init(void);

		void SetDiffuseColor(tVector color);
		void SetTransformation(const float m[16]);
};


#endif
