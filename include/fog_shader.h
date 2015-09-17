
#ifndef _FOG_SHADER_H
#define _FOG_SHADER_H


class tFogShader : public tScreenShader
{
	private:
		GLint depth_tex_uniform;
		GLint color_tex_uniform;

		GLint cam_pos_uniform;

		GLint start_dist_uniform;
		GLint end_dist_uniform;
		GLint exp_uniform;
		GLint color_uniform;

		int depth_tex_unit;
		int color_tex_unit;

	public:
		void Init(void);

		void SetTextures(GLuint depth_tex, GLuint color);
		void SetCameraPosition(tVector cam_pos);
		void SetFog(float start_dist, float end_dist, float exp, tVector color);
};


#endif
