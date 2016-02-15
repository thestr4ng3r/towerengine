
#ifndef _DEFAULT_FACE_SHADER_H
#define _DEFAULT_FACE_SHADER_H


class tGeometryPassShader : public tFaceShader
{
	private:
		GLint cam_pos_uniform;

		GLint transformation_uniform;

		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint specular_tex_uniform;
		GLint bump_tex_uniform;
		GLint self_illumination_tex_uniform;

		GLint cube_map_reflection_tex_uniform;

	public:
		void Init(void);

		void SetCameraPosition(tVector pos);
		void SetDiffuseTexture(bool enabled, GLuint tex);
		void SetSpecularTexture(GLuint tex);
		void SetNormalTexture(GLuint tex);
		void SetBumpTexture(GLuint tex);
		void SetTransformation(const float m[16]);
		void SetSelfIlluminationTexture(GLuint tex);
		void SetCubeMapReflectionTexture(GLuint tex);
};


#endif
