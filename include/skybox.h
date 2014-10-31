
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

		void SetCubeMap(GLuint cube_map)	{ this->cube_map = cube_map; }

		void Paint(tVector pos);
};


#endif
