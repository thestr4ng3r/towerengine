

#ifndef _MESH_MATERIAL_H
#define _MESH_MATERIAL_H

class tMaterialIBO
{
	public:
		tIBO *ibo;
		GLuint *ibo_data;

		int triangle_count; // TODO: maybe move this somewhere else
		int triangle_i;

		tMaterialIBO(tMesh *mesh);
		~tMaterialIBO(void);
};


#endif
