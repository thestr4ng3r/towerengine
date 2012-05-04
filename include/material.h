#ifndef _MATERIAL_H_
#define _MATERIAL_H_

struct CMaterial
{
	CMesh *mesh;
	CMaterial *chain_next;

	CTexture *tex;

	IBO *ibo;
	int triangle_count;
	int triangle_i;
	GLuint *ibo_data;

	char *name;
	char *diffuse_filename;
	char *specular_filename;
	char *normal_filename;
	char *aeb_filename;
	char *height_filename;
	float bump_factor;
	float height_factor;
	float specular_exponent;

	static CMaterial *current;

	CMaterial(CMesh *mesh);
	~CMaterial(void);

	void SetValues(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
			   float b_factor, const char *height, float h_factor);
	void Load(const char *path);

	static CMaterial *CreateMaterial(const char *diffuse, const char *specular, float exponent, const char *normal, const char *aeb,
			   float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh = 0);
	static CMaterial *CreateMaterialRelative(const char *path, const char *diffuse, const char *specular, float exponent, const char *normal,
			   const char *aeb, float b_factor, const char *height, float h_factor, char name[100], CMesh *mesh = 0);

	void PutToGL(void) { if(current != this) tex->PutToGL(); current = this; };
};

#endif
