
#ifndef _PARTICLE_SHADER_H
#define _PARTICLE_SHADER_H


class tParticleShader : public tShader
{
	private:
		GLint modelview_projection_matrix_uniform;

		GLint color_uniform;

	public:
		void Init(void);

		void SetModelViewProjectionMatrix(float m[16]);
		void SetColor(tVector color, float alpha);
};


#endif
