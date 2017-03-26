
#ifndef _SKYBOX_H
#define _SKYBOX_H

class tSkyBox
{
	private:
		GLuint cube_map;
		float size;

		tVAO *vao;
		tVBO<float> *vbo;

		tIBO *ibo;

	public:
		tSkyBox(GLuint cube_map = 0, float size = 50.0);
		~tSkyBox(void);

		void SetCubeMap(GLuint cube_map)	{ this->cube_map = cube_map; }
		GLuint GetCubeMap(void)				{ return cube_map; }

		void Render(tRenderer *renderer, tVector pos);
};


#endif
