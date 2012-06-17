#ifndef _MATERIAL_H_
#define _MATERIAL_H_

struct CMaterial
{
	CMesh *mesh;
	//CMaterial *chain_next;

	//CTexture *tex;

	IBO *ibo;
	int triangle_count;
	int triangle_i;
	GLuint *ibo_data;

	char *name;

	struct diffuse
	{
		bool enabled;
		string filename;
		GLuint tex;
		CVector color;
		CVector ambient_color;
	} diffuse;

	struct specular
	{
		bool enabled;
		string filename;
		float exponent;
		GLuint tex;
		CVector color;
	} specular;

	struct normal
	{
		bool enabled;
		string filename;
		GLuint tex;
	} normal;

	struct aeb
	{
		bool enabled;
		string filename;
		float bump_factor;
		GLuint tex;
	} aeb;

	struct height
	{
		bool enabled;
		string filename;
		float factor;
		GLuint tex;
	} height;

	bool transparent;

	static CMaterial *current;

	CMaterial(CMesh *mesh);
	~CMaterial(void);

	void SetValues(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb, float b_factor, const char *height, float h_factor);
	void Load(string path);

	static CMaterial *CreateMaterial(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
			   float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh = 0);
	static CMaterial *CreateMaterialRelative(const char *path, const char *diffuse, const char *specular, float exponent, const char *normal,
			   const char *aeb, float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh = 0);

	void PutToGL(void);// { if(current != this) tex->PutToGL(); current = this; };
};

#endif
