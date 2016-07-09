
#ifndef _CUBE_MAP_BLUR_SHADER_H
#define _CUBE_MAP_BLUR_SHADER_H

class tCubeMapBlurShader : public tShader
{
	private:
		GLint tex_uniform;
		GLint mipmap_level_uniform;
		GLint blur_dir_uniform;

	public:
		static const GLint vertex_attribute = 0;

		void Init(void);
		void SetTexture(GLuint tex);
		void SetBlurDir(tVector v);
		void SetMipmapLevel(int level);
};


#endif
