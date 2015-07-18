
#ifndef _PARTICLE_SHADER_H
#define _PARTICLE_SHADER_H


class tParticleShader : public tShader
{
	private:
		GLint modelview_matrix_uniform;
		GLint projection_matrix_uniform;

		GLint texture_uniform;

		int texture_unit;

	public:
		static const int size_attribute = 1;
		static const int rotation_attribute = 2;
		static const int color_attribute = 3;
		static const int texture_index_attribute = 4;

		void Init(tGBuffer *gbuffer);

		void SetModelViewProjectionMatrices(float modelview[16], float projection[16]);
		void SetTexture(GLuint texture);
};


#endif
