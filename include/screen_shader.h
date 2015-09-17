
#ifndef _SCREEN_SHADER_H
#define _SCREEN_SHADER_H


class tScreenShader : public tShader
{
	protected:
		void InitScreenShader(const char *frag_src, const char *shader_name);

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
