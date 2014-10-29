
#ifndef _COLOR_FACE_SHADER_H
#define _COLOR_FACE_SHADER_H


class CColorShader : public CFaceShader
{
	private:
		GLint transformation_uniform;

		GLint diffuse_color_uniform;

	public:
		void Init(void);

		void SetDiffuseColor(CVector color);
		void SetTransformation(const float m[16]);
		void ResetUniforms(void);
};


#endif
