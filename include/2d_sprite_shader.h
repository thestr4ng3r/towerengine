
#ifndef _2D_SPRITE_SHADER
#define _2D_SPRITE_SHADER


class t2DSpriteShader : public tShader
{
	private:
		GLint screen_size_uniform;

		GLint sprite_coords_uniform;

		GLint tex_uniform;

	public:
		void Init(void);

		void SetScreenSize(int width, int height);
		void SetSprite(int x, int y, int width, int height, GLuint tex);
};


#endif
