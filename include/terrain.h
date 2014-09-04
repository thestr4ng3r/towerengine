
#ifndef _TERRAIN_H
#define _TERRAIN_H

class CTerrainMaterial;

class CTerrain
{
	private:
		CHeightMap *height_map;

		int map_size;
		float size;
		float height;

		CTerrainMaterial *material;

		VAO *vao;
		IBO *ibo;
		VBO<float> *vertex_vbo;
		VBO<float> *normal_vbo;
		VBO<float> *tang_vbo;
		VBO<float> *bitang_vbo;
		VBO<float> *uvcoord_vbo;

		int *physics_triangles;
		float *physics_vertices;
		btCollisionShape *collision_shape;

	public:
		CTerrain(CHeightMap *height_map, float size = 10.0, float height = 1.0, CTerrainMaterial *material = 0);

		CBoundingBox GetBoundingBox(void);

		void SetMaterial(CTerrainMaterial *material) { this->material = material; }

		btCollisionShape *GetCollisionShape(void)	{ return collision_shape; }

		void Paint(void);

};

class CTerrainMaterial
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
		Diffuse diffuse;
		Specular specular;
		Normal normal;
		Height height;

		GLuint diffuse_tex;
		GLuint specular_tex;
		GLuint normal_tex;
		GLuint height_tex;

	public:
		CTerrainMaterial();
		~CTerrainMaterial(void);

		Diffuse GetDiffuse(void)	{ return diffuse; }
		Specular GetSpecular(void)	{ return specular; }
		Normal GetNormal(void)		{ return normal; }
		Height GetHeight(void)		{ return height; }

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
