
#ifndef _FOG_SHADER_H
#define _FOG_SHADER_H


class tFogShader : public tScreenShader
{
	private:
		GLint position_tex_uniform;
		GLint color_tex_uniform;

		GLint cam_pos_uniform;

		GLint start_dist_uniform;
		GLint end_dist_uniform;
		GLint exp_uniform;
		GLint color_uniform;

	public:
		void Init(void);

		void SetTextures(GLuint position, GLuint color);
		void SetCameraPosition(tVector cam_pos);
		void SetFog(float start_dist, float end_dist, float exp, tVector color);
};


#endif
