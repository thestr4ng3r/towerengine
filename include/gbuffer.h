
#ifndef _GBUFFER_H
#define _GBUFFER_H


class CGBuffer
{
	public:
		enum GBUFFER_TEXTURE_TYPE
		{
			POSITION_TEX,
			DIFFUSE_TEX,
			NORMAL_TEX,
			SPECULAR_TEX,
			SPECULAR_EXPONENT_TEX
		};

		static const int tex_count = 5;

	private:
		GLuint fbo;
		GLuint tex[tex_count];
		GLuint depth_tex;

		GLenum *draw_buffers;

	public:
		CGBuffer(int width, int height);
		~CGBuffer(void);

		void BindForDrawing(void);
		void BindForReading(void);

		GLuint GetTexture(GBUFFER_TEXTURE_TYPE type)	{ return tex[type]; }
		GLuint GetDepthTexture(void)					{ return depth_tex; }
};


#endif
