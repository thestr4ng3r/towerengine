
#ifndef _DEFAULT_FACE_SHADER_H
#define _DEFAULT_FACE_SHADER_H


class CDefaultFaceShader : public CFaceShader
{
	private:
		GLint transformation_uniform;

		GLint two_side_uniform;

		GLint diffuse_color_uniform;
		GLint diffuse_color2_uniform;
		GLint specular_color_uniform;
		GLint ambient_uniform;

		GLint point_light_count_uniform;
		GLint point_light_pos_uniform;
		GLint point_light_color_uniform;
		GLint point_light_distance_uniform;
		GLint point_light_shadow_enabled_uniform;
		GLint point_light_shadow_map_uniform;

		GLint directional_light_count_uniform;
		GLint directional_light_dir_uniform;
		GLint directional_light_color_uniform;
		GLint directional_light_shadow_enabled_uniform;
		GLint directional_light_shadow_clip_uniform;
		GLint directional_light_shadow_tex_matrix_uniform;
		GLint directional_light_shadow_map_uniform;

		GLint light_ambient_color_uniform;

		GLint specular_size_uniform; // TODO: in exponent umbenennen

		GLint diffuse_tex_enabled_uniform;
		GLint normal_tex_enabled_uniform;
		GLint height_tex_enabled_uniform;
		GLint specular_tex_enabled_uniform;

		GLint diffuse_tex_uniform;
		GLint normal_tex_uniform;
		GLint height_tex_uniform;
		GLint specular_tex_uniform;

		GLint tex_border_uniform;
		GLint discard_border_uniform;

		GLint height_factor_uniform;
		GLint bump_factor_uniform;

		GLint fog_color_uniform;
		GLint fog_depth_uniform;
		GLint fog_dist_uniform;

		GLint clip_uniform;
		GLint clip_dist_uniform;

		GLint vertex_mix_uniform;

	public:
		void Init(void);

		void SetClip(CVector c, float d);
		void SetDiffuseColor(CVector color);
		void SetDiffuseColor2(CVector color, float alpha);
		void SetSpecularColor(CVector color);
		void SetAmbient(float ambient);
		void SetPointLights(int count, float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_maps);
		void SetDirectionalLights(int count, float *dir, float *color, int *shadow_enabled, GLuint *shadow_maps, float *shadow_clip, float *shadow_tex_martix);
		void SetLightAmbientColor(CVector color);
		void SetSpecular(float size);
		void SetDiffuseTexture(bool enabled, GLuint tex = 0);
		void SetSpecularTexture(bool enabled, GLuint tex = 0);
		void SetNormalTexture(bool enabled, GLuint tex = 0);
		void SetHeightTexture(bool enabled, GLuint tex = 0);
		void SetBorder(int e, CVector2 a, CVector2 b);
		void SetTexCoord(CVector2 coord);
		void SetVectors(CVector normal, CVector tangx, CVector tangy, CVector fnormal = Vec(0.0, 0.0, 0.0));
		void SetHeightFactor(CVector factor);
		void SetBumpFactor(float f);
		void SetFog(CVector color, float depth, float dist);
		void SetTwoSide(int v);
		void SetTransformation(const float m[16]);
		void SetVertexMix(float m = 0.0);
		void ResetUniforms(void);
};


#endif
