
#ifndef _REFRACTION_MATERIAL_H
#define _REFRACTION_MATERIAL_H


class tRefractionMaterial : public tMaterial
{
	protected:
		tVector color;
		GLuint color_tex;

		GLuint normal_tex;

	public:
		tRefractionMaterial(void);
		~tRefractionMaterial(void);

		void SetColor(tVector color);

		void LoadColorTexture(std::string file);
		void LoadColorTexture(const char *extension, const void *data, unsigned int size);

		void LoadNormalTexture(std::string file);
		void LoadNormalTexture(const char *extension, const void *data, unsigned int size);

		bool InitRefractionPass(tRenderer *renderer, float *transform);
};

#endif
