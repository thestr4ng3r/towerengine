

#ifndef _MESH_MATERIAL_H
#define _MESH_MATERIAL_H

class tMeshMaterial : public tMaterial
{
	private:
		tMesh *mesh;
		string name;

	public:
		tIBO *ibo;
		int triangle_count;
		int triangle_i;
		GLuint *ibo_data;

		tMeshMaterial(tMesh *mesh, string name);
		~tMeshMaterial(void);

		string GetName(void)		{ return name; }
		tMesh *GetMesh(void)		{ return mesh; }
};


#endif
