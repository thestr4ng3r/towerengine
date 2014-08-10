
#ifndef _FACE_SHADER_H
#define _FACE_SHADER_H


class CFaceShader : public CShader
{
	public:
		static const GLint vertex_attribute = 0;
		static const GLint vertex2_attribute = 1;
		static const GLint normal_attribute = 2;
		static const GLint tang_attribute = 3;
		static const GLint bitang_attribute = 4;
		static const GLint uvcoord_attribute = 5;

		static const GLint max_point_lights = 8;
		static const GLint max_directional_lights = 8;

		static const unsigned int diffuse_tex_unit = 0;
		static const unsigned int specular_tex_unit = 4;
		static const unsigned int normal_tex_unit = 2;
		static const unsigned int height_tex_unit = 3;
		static const unsigned int point_light_shadow_tex_first_unit = 5;
		static const unsigned int directional_light_shadow_tex_first_unit = point_light_shadow_tex_first_unit + max_point_lights;

		virtual void SetClip(CVector c, float d) {};
		virtual void SetDiffuseColor(CVector color) {};
		virtual void SetDiffuseColor2(CVector color, float alpha) {};
		virtual void SetSpecularColor(CVector color) {};
		virtual void SetAmbient(float ambient) {};
		virtual void SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps) {};
		virtual void SetDirectionalLights(int count, float *dir, float *color, int *shadow_enabled, GLuint *shadow_maps, float *shadow_clip, float *shadow_tex_martix) {};
		virtual void SetLightAmbientColor(CVector color) {};
		virtual void SetSpecular(float size) {};
		virtual void SetDiffuseTexture(bool enabled, GLuint tex = 0) {};
		virtual void SetSpecularTexture(bool enabled, GLuint tex = 0) {};
		virtual void SetNormalTexture(bool enabled, GLuint tex = 0) {};
		virtual void SetHeightTexture(bool enabled, GLuint tex = 0) {};
		virtual void SetBorder(int e, CVector2 a, CVector2 b) {};
		virtual void SetTexCoord(CVector2 coord) {};
		virtual void SetVectors(CVector normal, CVector tangx, CVector tangy, CVector fnormal = Vec(0.0, 0.0, 0.0)) {};
		virtual void SetHeightFactor(CVector factor) {};
		virtual void SetBumpFactor(float f) {};
		virtual void SetFog(CVector color, float depth, float dist) {};
		virtual void SetTwoSide(int v) {};
		virtual void SetTransformation(const float m[16]) {};
		virtual void SetVertexMix(float m = 0.0) {};
		virtual void ResetUniforms(void) {};
};

#endif
