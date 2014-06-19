
#ifndef _POINT_SHADOW_SHADER_H
#define _POINT_SHADOW_SHADER_H

class CPointShadowShader : public CShader
{
	private:
		GLint light_pos_uniform;
		GLint light_dist_uniform;

	public:
		void Init(void);
		void SetLightPos(CVector v);
		void SetLightDist(float d);
		void ResetUniforms(void) {};
};

extern CPointShadowShader cube_env_shader;

#endif
