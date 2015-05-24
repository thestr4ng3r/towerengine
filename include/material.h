#ifndef _MATERIAL_H
#define _MATERIAL_H

class tMaterial
{
	protected:
		struct Diffuse
		{
			GLuint tex;
			tVector color;
		} diffuse;

		struct Specular
		{
			GLuint tex;
			float exponent;
			tVector color;
		} specular;

		struct Normal
		{
			GLuint tex;
		} normal;

		struct SelfIllumination
		{
			tVector color;
			GLuint tex;
		} self_illum;

		struct Bump
		{
			bool enabled;
			GLuint tex;
			float depth;
		} bump;

		struct CubeMapReflection
		{
			bool enabled;
			tVector color;
		} cube_map_reflection;

		bool transparent;

	public:
		tMaterial(void);
		~tMaterial(void);

		void SetDiffuse(tVector color);
		void SetSpecular(tVector color, float exponent);
		void SetBump(float depth);
		void SetSelfIlluminationColor(tVector color);
		void SetCubeMapReflection(bool enabled, tVector color);

		void LoadDiffuseTexture(std::string file);
		void LoadSpecularTexture(std::string file);
		void LoadNormalTexture(std::string file);
		void LoadBumpTexture(std::string file);
		void LoadSelfIlluminationTexture(std::string file);

		void LoadDiffuseTexture(const char *extension, const void *data, unsigned int size);
		void LoadSpecularTexture(const char *extension, const void *data, unsigned int size);
		void LoadNormalTexture(const char *extension, const void *data, unsigned int size);
		void LoadBumpTexture(const char *extension, const void *data, unsigned int size);
		void LoadSelfIlluminationTexture(const char *extension, const void *data, unsigned int size);

		void InitGeometryPass(tRenderer *renderer);

		bool GetTransparent(void)	{ return transparent; }

		bool GetCubeMapReflectionEnabled(void)	{ return cube_map_reflection.enabled; }
		tVector GetCubeMapReflectionColor(void)	{ return cube_map_reflection.color; }
};

#endif
