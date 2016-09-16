
#ifndef _POINT_SHADOW_SHADER_H
#define _POINT_SHADOW_SHADER_H

class tPointShadowShader : public tFaceShader
{
	private:
		GLint modelview_projection_matrix_uniform;

		GLint light_pos_uniform;
		GLint light_dist_uniform;

		GLint transformation_uniform;

		GLint diffuse_tex_enabled_uniform;
		GLint diffuse_tex_uniform;

	public:
		tPointShadowShader(void);

		void SetModelViewProjectionMatrix(float m[16]);
		void SetLightPos(tVector v);
		void SetLightDist(float d);
		void SetTransformation(const float m[16]);
		void SetBaseColorTexture(bool enabled, GLuint tex = 0);
};

class tPointShadowBlurShader : public tShader
{
	private:
		GLint tex_uniform;
		GLint blur_dir_uniform;

	public:
		static const GLint vertex_attribute = 0;

		tPointShadowBlurShader(void);
		void SetTexture(GLuint tex);
		void SetBlurDir(tVector v);
};

#endif
