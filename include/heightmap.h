#ifndef _HEIGHTMAP_H
#define _HEIGHTMAP_H

class tHeightMap
{
	private:
		int size;
		float *data;

		GLuint height_tex;

	public:
		tHeightMap(int size);

		bool Load(const char *file);

		GLuint GetHeightTex(void)		{ return height_tex; }
		float GetValue(int x, int y) 	{ return data[std::min(std::max(y, 0), size) * size + std::min(std::max(x, 0), size)]; }
		float *GetData(void)			{ return data; };
		int GetSize(void) 				{ return size; }
};

#endif
