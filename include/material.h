#ifndef _MATERIAL_H
#define _MATERIAL_H

class tMaterial
{
	protected:
		struct Diffuse
		{
			bool enabled;
			GLuint tex;
			tVector color;
		} diffuse;

		struct Specular
		{
			bool enabled;
			GLuint tex;
			float exponent;
			tVector color;
		} specular;

		struct Normal
		{
			bool enabled;
			GLuint tex;
		} normal;

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
		void SetCubeMapReflection(bool enabled, tVector color);

		void LoadDiffuseTextureURL(string file);
		void LoadSpecularTextureURL(string file);
		void LoadNormalTextureURL(string file);
		void LoadBumpTextureURL(string file);

		void LoadDiffuseTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadSpecularTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadNormalTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadBumpTextureBinary(const char *extension, const void *data, unsigned int size);

		void InitGeometryPass(tRenderer *renderer);
		void InitCubeMapReflectionPass(tRenderer *renderer);

		bool GetTransparent(void)	{ return transparent; }

		bool GetCubeMapReflectionEnabled(void)	{ return cube_map_reflection.enabled; }
		tVector GetCubeMapReflectionColor(void)	{ return cube_map_reflection.color; }
};

#endif
