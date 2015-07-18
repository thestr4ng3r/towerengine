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
#ifdef _WIN32
		float GetValue(int x, int y) 	{ return data[min(max(y, 0), size) * size + min(max(x, 0), size)]; }
#else
		float GetValue(int x, int y) 	{ return data[std::min(std::max(y, 0), size) * size + std::min(std::max(x, 0), size)]; }
#endif
		float *GetData(void)			{ return data; };
		int GetSize(void) 				{ return size; }
};

#endif
