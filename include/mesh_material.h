

#ifndef _MESH_MATERIAL_H
#define _MESH_MATERIAL_H

class CMeshMaterial : public CMaterial
{
	private:
		CMesh *mesh;
		string name;

	public:
		IBO *ibo;
		int triangle_count;
		int triangle_i;
		GLuint *ibo_data;

		CMeshMaterial(CMesh *mesh, string name);
		~CMeshMaterial(void);

		string GetName(void)		{ return name; }
		CMesh *GetMesh(void)		{ return mesh; }
};


#endif
