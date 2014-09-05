#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class CMaterial
{
	public:
		struct Diffuse
		{
			bool enabled;
			GLuint tex;
			CVector color;
			float ambient;
		} diffuse;

		struct Specular
		{
			bool enabled;
			GLuint tex;
			float exponent;
			CVector color;
		} specular;

		struct Normal
		{
			bool enabled;
			GLuint tex;
		} normal;

		CMesh *mesh;

		string name;

		bool transparent;

	public:
		IBO *ibo;
		int triangle_count;
		int triangle_i;
		GLuint *ibo_data;

		CMaterial(CMesh *mesh, string name);
		~CMaterial(void);

		string GetName(void)		{ return name; }
		CMesh *GetMesh(void)		{ return mesh; }
		bool GetTransparent(void)	{ return transparent; }

		void SetDiffuse(CVector color, float ambient);
		void SetSpecular(CVector color, float exponent);

		void LoadDiffuseTextureURL(string file);
		void LoadSpecularTextureURL(string file);
		void LoadNormalTextureURL(string file);

		void LoadDiffuseTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadSpecularTextureBinary(const char *extension, const void *data, unsigned int size);
		void LoadNormalTextureBinary(const char *extension, const void *data, unsigned int size);

		void PutToGL(void);
};

#endif
