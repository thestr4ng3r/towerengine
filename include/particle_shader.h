
#ifndef _PARTICLE_SHADER_H
#define _PARTICLE_SHADER_H


class tParticleShader : public tShader
{
	private:
		GLint modelview_matrix_uniform;
		GLint projection_matrix_uniform;

		GLint color_uniform;

		GLint texture_uniform;

		int texture_unit;

	public:
		static const int size_attribute = 1;

		void Init(tGBuffer *gbuffer);

		void SetModelViewProjectionMatrices(float modelview[16], float projection[16]);
		void SetColor(tVector color, float alpha);
		void SetTexture(GLuint texture);
};


#endif
