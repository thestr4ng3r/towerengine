
#ifndef _REFRACTION_MATERIAL_H
#define _REFRACTION_MATERIAL_H


class tRefractionMaterial : public tMaterial
{
	protected:
		tVector color;
		tVector edge_color;
		float edge_alpha;

		GLuint color_tex;

		GLuint normal_tex;

	public:
		tRefractionMaterial(void);
		~tRefractionMaterial(void);

		void SetColor(tVector color);
		void SetEdgeColor(tVector edge_color, float alpha);

		void LoadColorTexture(std::string file);
		void LoadColorTexture(const char *extension, const void *data, unsigned int size);
		void SetColorTexture(GLuint gl_tex);

		void LoadNormalTexture(std::string file);
		void LoadNormalTexture(const char *extension, const void *data, unsigned int size);
		void SetNormalTexture(GLuint gl_tex);

		bool InitRefractionPass(tRenderer *renderer, float *transform);
};

#endif
