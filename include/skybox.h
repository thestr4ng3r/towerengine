
#ifndef _SKYBOX_H
#define _SKYBOX_H

class CSkyBox
{
	private:
		GLuint cube_map;

	public:
		CSkyBox(GLuint cube_map = 0)		{ SetCubeMap(cube_map); };

		void SetCubeMap(GLuint cube_map)	{ this->cube_map = cube_map; };

		void Paint(CVector pos, float size = 100.0);
};


#endif
