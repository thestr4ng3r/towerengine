
#ifndef _SCREEN_SHADER_H
#define _SCREEN_SHADER_H


class tScreenShader : public tShader
{
	protected:
		void InitScreenShader(const char *frag_src, const char *shader_name);

	public:
		static const int uv_coord_attribute = 1;
};

class tReflectingShader
{
	public:
		virtual void SetReflectionTextures(GLuint tex1, GLuint tex2, float blend) =0;
};


class tPositionRestoreDataBuffer : public tUniformBuffer
{
	public:
		tPositionRestoreDataBuffer(void);
		~tPositionRestoreDataBuffer(void);

		void UpdateBuffer(tCamera *camera, float uv_factor_x = 2.0f, float uv_offset_x = -0.5f);

		void Bind(void);
};



#endif
