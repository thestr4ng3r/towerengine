
#ifndef _ASSET_H
#define _ASSET_H

#define T_ASSET_TYPE_MESH		0
#define T_ASSET_TYPE_CUBEMAP	1

class tAsset
{
	public:
		virtual ~tAsset(void)	{};

		virtual int GetType(void) =0;
};

class tMeshAsset : public tAsset
{
	private:
		tMesh *mesh;

	public:
		tMeshAsset(tMesh *mesh);
		~tMeshAsset(void);

		tMesh *GetMesh(void)	{ return mesh; }

		int GetType(void)		{ return T_ASSET_TYPE_MESH; }
};

class tCubeMapAsset : public tAsset
{
	private:
		GLuint cubemap;

	public:
		tCubeMapAsset(GLuint cubemap);
		~tCubeMapAsset(void);

		GLuint GetCubeMap(void)	{ return cubemap; }

		int GetType(void)		{ return T_ASSET_TYPE_CUBEMAP; }
};

#endif
