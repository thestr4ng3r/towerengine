
#ifndef _DEFAULT_FACE_SHADER_H
#define _DEFAULT_FACE_SHADER_H


class tGeometryPassShader : public tFaceShader
{
	private:
		GLint transformation_uniform;

		GLint diffuse_color_uniform;
		GLint diffuse_color2_uniform;
		GLint specular_color_uniform;

		GLint specular_size_uniform; // TODO: in exponent umbenennen

		GLint bump_depth_uniform;

		GLint diffuse_tex_enabled_uniform;
		GLint normal_tex_enabled_uniform;
		GLint specular_tex_enabled_uniform;
		GLint bump_tex_enabled_uniform;

		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;
		GLint bump_tex_uniform;

		GLint clip_uniform;
		GLint clip_dist_uniform;

		GLint vertex_mix_uniform;

	public:
		void Init(void);

		void SetClip(tVector c, float d);
		void SetDiffuseColor(tVector color);
		void SetDiffuseColor2(tVector color, float alpha);
		void SetSpecularColor(tVector color);
		void SetSpecular(float size);
		void SetDiffuseTexture(bool enabled, GLuint tex = 0);
		void SetSpecularTexture(bool enabled, GLuint tex = 0);
		void SetNormalTexture(bool enabled, GLuint tex = 0);
		void SetBumpTexture(bool enabled, GLuint tex = 0);
		void SetTexCoord(tVector2 coord);
		void SetVectors(tVector normal, tVector tangx, tVector tangy, tVector fnormal = Vec(0.0, 0.0, 0.0));
		void SetBumpDepth(float depth);
		void SetTransformation(const float m[16]);
		void SetVertexMix(float m = 0.0);
		void ResetUniforms(void);
};


#endif
