
#ifndef _GBUFFER_H
#define _GBUFFER_H


class tGBuffer
{
	public:
		enum GBUFFER_TEXTURE_TYPE
		{
			POSITION_TEX,
			DIFFUSE_TEX,
			NORMAL_TEX,
			FACE_NORMAL_TEX,
			SPECULAR_TEX,
			SELF_ILLUMINATION_TEX
		};

		static const int tex_count = 6;

	private:
		GLuint fbo;
		GLuint tex[tex_count];

		int first_attachment;

		GLenum *draw_buffers;
		int *tex_units;

		void CreateTexImage(GBUFFER_TEXTURE_TYPE type, int width, int height);

	public:
		tGBuffer(int width, int height, GLuint fbo, int first_attachment);
		~tGBuffer(void);

		void ChangeSize(int width, int height);

		void BindDrawBuffers(void);
		void BindTextures(void);

		GLuint GetTexture(GBUFFER_TEXTURE_TYPE type)		{ return tex[type]; }
		int GetTextureUnit(GBUFFER_TEXTURE_TYPE type)		{ return tex_units[type]; }
		int GetLastTextureUnit(void)						{ return tex_count-1; }

		static int GetTexCount(void)						{ return tex_count; }
};


#endif
