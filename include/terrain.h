
#ifndef _TERRAIN_H
#define _TERRAIN_H

class tTerrainMaterial;

class tTerrain
{
	private:
		tHeightMap *height_map;

		int map_size;
		float size;
		float height;

		tTerrainMaterial *material;

		tVAO *vao;
		tIBO *ibo;
		tVBO<float> *vertex_vbo;
		tVBO<float> *normal_vbo;
		tVBO<float> *tang_vbo;
		tVBO<float> *bitang_vbo;
		tVBO<float> *uvcoord_vbo;

		int *physics_triangles;
		float *physics_vertices;
		btCollisionShape *collision_shape;

	public:
		tTerrain(tHeightMap *height_map, float size = 10.0, float height = 1.0, tTerrainMaterial *material = 0);

		tAABB GetAABB(void);

		void SetMaterial(tTerrainMaterial *material) { this->material = material; }

		btCollisionShape *GetCollisionShape(void)	{ return collision_shape; }

		void Paint(tDeferredRenderer *renderer);

};

class tTerrainMaterial : public tMaterial
{
};

#endif
