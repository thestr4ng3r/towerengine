
#ifndef _SCREEN_SHADER_H
#define _SCREEN_SHADER_H


class tScreenShader : public tShader
{
	protected:
		void InitScreenShader(const char *frag_src, const char *shader_name);

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

		void UpdateBuffer(tCamera *camera);

		void Bind(void);
};



#endif
