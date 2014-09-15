
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

class CTerrainMaterial : public CMaterial
{
};

#endif
