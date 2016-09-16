
#ifndef _PARTICLE_SHADER_H
#define _PARTICLE_SHADER_H

class tParticleShader : public tShader
{
	private:
		GLint modelview_matrix_uniform;
		GLint projection_matrix_uniform;

		GLint texture_uniform;

		int texture_unit;

	protected:
		tParticleShader(tGBuffer *gbuffer, const char *vertex_source, const char *geometry_source, const char *fragment_source);

	public:
		static const int size_attribute = 1;
		static const int rotation_attribute = 2;
		static const int color_attribute = 3;
		static const int texture_index_attribute = 4;

		void SetModelViewProjectionMatrices(float modelview[16], float projection[16]);
		void SetTexture(GLuint texture);
};

class tParticleForwardShader : public tParticleShader
{
	public:
		tParticleForwardShader(tGBuffer *gbuffer);
};

class tParticleDeferredShader : public tParticleShader
{
	private:
		GLint face_normal_uniform;
		GLint lighting_normal_uniform;

	public:
		tParticleDeferredShader(tGBuffer *gbuffer);

		void SetFaceNormal(tVector normal);
		void SetLightingNormal(tVector normal);
};


#endif
