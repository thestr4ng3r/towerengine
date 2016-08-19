
#ifndef _FACE_SHADER_H
#define _FACE_SHADER_H


class tFaceShader : public tShader
{
	public:
		static const GLint normal_attribute = 1;
		static const GLint tang_attribute = 2;
		static const GLint bitang_attribute = 3;
		static const GLint uvcoord_attribute = 4;

		static const unsigned int base_color_tex_unit = 0;
		static const unsigned int metallic_roughness_tex_unit = 1;
		static const unsigned int normal_tex_unit = 2;
		static const unsigned int bump_tex_unit = 3;
		static const unsigned int emission_tex_unit = 4;

		virtual void SetBaseColorTexture(bool enabled, GLuint tex) {}
		virtual void SetMetallicRoughnessTexture(GLuint tex) {}
		virtual void SetNormalTexture(GLuint tex)	{}
		virtual void SetBumpTexture(GLuint tex) {}
		virtual void SetEmissionTexture(GLuint tex) {}


		/*virtual void SetSpecularTexture(GLuint tex) {}
		virtual void SetNormalTexture(GLuint tex) {}
		virtual void SetBumpTexture(GLuint tex) {}
		virtual void SetSelfIlluminationTexture(GLuint tex = 0) {}*/
		virtual void SetCubeMapReflectionTexture(GLuint tex) {}

		virtual void SetTransformation(const float m[16]) {}
};


#endif
