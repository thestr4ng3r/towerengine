
#ifndef _SIMPLE_FORWARD_MATERIAL_H
#define _SIMPLE_FORWARD_MATERIAL_H


class tSimpleForwardMaterial : public tMaterial
{
	public:
		enum BlendMode { ALPHA, ADD, MULTIPLY };

	protected:
		tVector color; float alpha;
		GLuint tex;

		BlendMode blend_mode;

	public:
		tSimpleForwardMaterial(void);
		~tSimpleForwardMaterial(void);

		void SetColor(tVector color, float alpha);
		void SetBlendMode(BlendMode blend_mode);

		void LoadTexture(std::string file);
		void LoadTexture(const char *extension, const void *data, unsigned int size);
		void SetTexture(GLuint gl_tex);

		bool InitForwardPass(tDeferredRenderer *renderer, float *transform);
};

#endif
