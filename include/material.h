#ifndef _MATERIAL_H
#define _MATERIAL_H

class tMaterial
{
	public:
		virtual ~tMaterial(void) {}

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
			DIFFUSE,
			SPECULAR,
			NORMAL,
			SELF_ILLUMINATION,
			BUMP
		};

		static const int tex_count = 5;

	protected:
		struct Diffuse
		{
			tVector color;
		} diffuse;

		struct Specular
		{
			float exponent;
			tVector color;
		} specular;

		struct SelfIllumination
		{
			tVector color;
		} self_illum;

		struct Bump
		{
			bool enabled;
			float depth;
		} bump;

		struct CubeMapReflection
		{
			bool enabled;
			tVector color;
		} cube_map_reflection;

		//bool transparent;

		GLuint tex[tex_count];
		//GLuint64 tex_handle[tex_count];

		bool tex_handles_resident;

	public:
		tDefaultMaterial(void);
		~tDefaultMaterial(void);

		void SetDiffuse(tVector color);
		void SetSpecular(tVector color, float exponent);
		void SetBump(float depth);
		void SetSelfIlluminationColor(tVector color);
		void SetCubeMapReflection(bool enabled, tVector color);

		void LoadTexture(TextureType type, std::string file);
		void LoadTexture(TextureType type, const char *extension, const void *data, unsigned int size);

		//void MakeTextureHandlesResident(bool resident);
		//GLuint64 GetTextureHandle(TextureType type)		{ return tex_handle[type]; }

		//bool GetTransparent(void)	{ return transparent; }

		bool InitGeometryPass(tRenderer *renderer);

		bool GetCubeMapReflectionEnabled(void)	{ return cube_map_reflection.enabled; }
		tVector GetCubeMapReflectionColor(void)	{ return cube_map_reflection.color; }
};

#endif
