
#ifndef _GBUFFER_H
#define _GBUFFER_H


class tGBuffer
{
	public:
		enum BufferType
		{
			POSITION_TEX,
			NORMAL_TEX,
			TANG_TEX,
			UV_TEX,
			MATERIAL_TEX
		};

		static const int tex_count = 5;

	private:
		GLuint fbo;
		GLuint tex[tex_count];

		int first_attachment;

		GLenum *draw_buffers;
		int *tex_units;

		void CreateTexImage(BufferType type, int width, int height);

	public:
		tGBuffer(int width, int height, GLuint fbo, int first_attachment);
		~tGBuffer(void);

		void ChangeSize(int width, int height);

		void BindDrawBuffers(void);
		void BindTextures(void);

		GLuint GetTexture(BufferType type)		{ return tex[type]; }
		int GetTextureUnit(BufferType type)		{ return tex_units[type]; }
		int GetLastTextureUnit(void)						{ return tex_count-1; }

		static int GetTexCount(void)						{ return tex_count; }
};


#endif
