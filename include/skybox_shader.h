#ifndef _SKYBOX_SHADER_H
#define _SKYBOX_SHADER_H

class tSkyBoxShader : public tShader
{
	private:
		GLint cam_pos_uniform;

		GLint tex_uniform;
	
	public:
		static const int vertex_attribute = 0;

		tSkyBoxShader(void);
		void SetCubeMap(GLuint tex);
		void SetCameraPosition(tVector pos);
};

GLenum CubeTex(int side);
int CubeSide(GLenum tex);
tVector CubeVecS(int side);


#endif
