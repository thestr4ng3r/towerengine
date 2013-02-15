#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class CMaterial
{
	public:
		struct Diffuse
		{
			bool enabled;
			string filename;
			CVector color;
			float ambient;
		};

		struct Specular
		{
			bool enabled;
			string filename;
			float exponent;
			CVector color;
		};

		struct Normal
		{
			bool enabled;
			string filename;
		};

		struct Height
		{
			bool enabled;
			string filename;
			float factor;
		};

	private:
		CMesh *mesh;

		string name;

		Diffuse diffuse;
		Specular specular;
		Normal normal;
		Height height;

		GLuint diffuse_tex;
		GLuint specular_tex;
		GLuint normal_tex;
		GLuint height_tex;

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
		Diffuse GetDiffuse(void)	{ return diffuse; }
		Specular GetSpecular(void)	{ return specular; }
		Normal GetNormal(void)		{ return normal; }
		Height GetHeight(void)		{ return height; }
		bool GetTransparent(void)	{ return transparent; }

		void SetDiffuse(bool enabled, string filename = string(), CVector color = Vec(1.0, 1.0, 1.0), float ambient = 1.0);
		void SetSpecular(bool enabled, string filename = string(), CVector color = Vec(1.0, 1.0, 1.0), float exponent = 32.0);
		void SetNormal(bool enabled, string filename = string());
		void SetHeight(bool enabled, string filename = string(), float factor = 1.0);

		void Load(string path);

		static CMaterial *CreateMaterial(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
				   float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh = 0);

		void PutToGL(void);
};

#endif
