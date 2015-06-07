
#ifndef _2D_RENDERER_H
#define _2D_RENDERER_H


class t2DRenderer
{
	private:
		t2DSpriteShader *sprite_shader;

		tVAO *vao;
		tVBO<float> *vertex_vbo;
		tIBO *ibo;

	public:
		t2DRenderer(void);
		~t2DRenderer(void);

		void InitRender(int screen_width, int screen_height);

		void RenderSprite(int x, int y, int width, int height, GLuint tex);
};



#endif
