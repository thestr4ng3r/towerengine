#ifndef _SHADER_H
#define _SHADER_H

class tShader
{
	private:
		const char *name;
		bool status;

	protected:
		tShader(void) {}

		GLuint program;

		static GLuint CreateShaderFromSource(GLenum type, const char *src);
		static GLuint CreateShaderFromNamedString(GLenum type, const char *name);
		void CompileAndAttachShader(GLuint shader);
		void LinkProgram(void);

		void InitShader(const char *vert_src, const char *frag_src, const char *shader_name = 0);
		GLint GetUniformLocation(const char *uniform);
	
	public:
		static const int vertex_attribute = 0;

		static const int matrix_binding_point = 0;
		static const int material_binding_point = 1;
		static const int position_restore_data_binding_point = 2;
		static const int point_light_binding_point = 3;

		static void InitSource(void);

		void Bind(void);
		static void Unbind(void);

		virtual ~tShader(void) {};

		bool GetStatus(void)	{ return status; }
};


class tUniformBuffer
{
	protected:
		unsigned int size;
		unsigned char *data;

		GLuint buffer;

	public:
		tUniformBuffer(unsigned int size);
		virtual ~tUniformBuffer(void);

		unsigned char *GetData(void)	{ return data; }

		void UploadData(GLenum usage = GL_DYNAMIC_DRAW);
		void Bind(GLuint binding_point);
};



class tMatrixBuffer : public tUniformBuffer
{
	public:
		tMatrixBuffer(void);
		~tMatrixBuffer(void);

		void UpdateBuffer(const tMatrix4 &matrix);

		void Bind(void);
};


GLuint LoadGLTexture(const char *filename, int channels = 4, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);
GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels = 4, int *w = 0, int *h = 0, bool *transparent = 0, int alpha_channel = 3);

GLuint LoadGLTextureArray(const char **filenames, const int count, int *w = 0, int *h = 0);

inline GLuint LoadGLTexture(const char *filename, int channels, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTexture(filename, channels, 0, 0, transparent, alpha_channel); }
inline GLuint LoadGLTextureBinary(const char *ext, const void *data, size_t size, int channels, bool *transparent, int alpha_channel = 3)
		{ return LoadGLTextureBinary(ext, data, size, channels, 0, 0, transparent, alpha_channel); }


#endif
