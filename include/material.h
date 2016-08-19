#ifndef _MATERIAL_H
#define _MATERIAL_H

class tMaterial
{
	public:
		virtual ~tMaterial(void) {}

		virtual bool InitDepthPrePass(tRenderer *renderer)						{ return false; }
		virtual bool InitGeometryPass(tRenderer *renderer)						{ return false; }
		virtual bool InitForwardPass(tRenderer *renderer, float *transform)		{ return false; }
		virtual bool InitRefractionPass(tRenderer *renderer, float *transform)	{ return false; }

		virtual bool GetCubeMapReflectionEnabled(void)	{ return false; }
		virtual tVector GetCubeMapReflectionColor(void)	{ return Vec(0.0, 0.0, 0.0); }
};

class tDefaultMaterial : public tMaterial
{
	public:
		enum TextureType
		{
			BASE_COLOR,
			METAL_ROUGH_REFLECT,
			NORMAL,
			EMISSION,
			BUMP
		};

		static const int tex_count = 5;

	protected:
		tVector base_color;
		float metallic;
		float roughness;
		float reflectance;

		tVector emission;

		float bump_depth;


		bool shadow_cast;


		bool transparent;

		GLuint tex[tex_count];


		tUniformBuffer *uniform_buffer;

	public:
		tDefaultMaterial(void);
		~tDefaultMaterial(void);

		void SetBaseColor(tVector color)	{ this->base_color = color; }
		void SetMetallic(float metallic)	{ this->metallic = metallic; }
		void SetRoughness(float roughness)	{ this->roughness = roughness; }
		void SetReflectance(float reflect)	{ this->reflectance = reflect; }
		void SetEmission(tVector color)		{ this->emission = color; }
		void SetBumpDepth(float depth)		{ this->bump_depth = depth; }
		void SetShadowCast(bool enabled)	{ this->shadow_cast = enabled; }

		void LoadTexture(TextureType type, std::string file);
		void LoadTexture(TextureType type, const char *extension, const void *data, size_t size);

		void UpdateUniformBuffer(void);

		bool InitDepthPrePass(tRenderer *renderer);
		bool InitGeometryPass(tRenderer *renderer);
};

#endif
