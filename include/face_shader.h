
#ifndef _FACE_SHADER_H
#define _FACE_SHADER_H


class tFaceShader : public tShader
{
	public:
		static const GLint normal_attribute = 1;
		static const GLint tang_attribute = 2;
		static const GLint bitang_attribute = 3;
		static const GLint uvcoord_attribute = 4;

		static const unsigned int diffuse_tex_unit = 0;
		static const unsigned int specular_tex_unit = 1;
		static const unsigned int normal_tex_unit = 2;
		static const unsigned int bump_tex_unit = 3;
		static const unsigned int self_illumination_tex_unit = 4;
		static const unsigned int cube_map_reflection_tex_unit = 5;

		//virtual void SetClip(tVector c, float d) {}
		virtual void SetDiffuseColor(tVector color) {}
		virtual void SetSpecularColor(tVector color) {}
		virtual void SetBumpDepth(float depth) {}
		virtual void SetSpecular(float size) {}
		virtual void SetDiffuseTexture(bool enabled, GLuint tex = 0) {}
		virtual void SetSpecularTexture(bool enabled, GLuint tex = 0) {}
		virtual void SetNormalTexture(bool enabled, GLuint tex = 0) {}
		virtual void SetBumpTexture(bool enabled, GLuint tex = 0) {}
		virtual void SetTransformation(const float m[16]) {}
		virtual void SetSelfIlluminationColor(tVector color) {}
		virtual void SetSelfIlluminationTexture(bool enabled, GLuint tex = 0) {}
		virtual void SetCubeMapReflectionEnabled(bool enabled) {}
		virtual void SetCubeMapReflectionColor(tVector color) {}
		virtual void SetCubeMapReflectionTexture(GLuint tex) {}
};


#endif
