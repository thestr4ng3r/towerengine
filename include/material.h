#ifndef _MATERIAL_H_
#define _MATERIAL_H_

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

		bool transparent;

	public:
		tMaterial(void);
		~tMaterial(void);

		bool GetTransparent(void)	{ return transparent; }

		void SetDiffuse(tVector color);
		void SetSpecular(tVector color, float exponent);
		void SetBump(float depth);

		void LoadDiffuseTextureURL(string file);
		void LoadSpecularTextureURL(string file);
		void LoadNormalTextureURL(string file);
		void LoadBumpTextureURL(string file);

		void LoadDiffuseTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadSpecularTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadNormalTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadBumpTextureBinary(const char *extension, const void *data, unsigned int size);

		void PutToGL(void);
};

#endif
