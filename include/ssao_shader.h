
#ifndef _SSAO_SHADER_H
#define _SSAO_SHADER_H

#define MAX_SSAO_KERNEL_SIZE 32

class CSSAOShader : public CShader
{
	private:
		GLint kernel_uniform;
		GLint kernel_size_uniform;
		GLint noise_tex_uniform;
		GLint noise_tex_scale_uniform;

		GLint depth_tex_uniform;
		GLint position_tex_uniform;
		GLint normal_tex_uniform;

		GLint projection_matrix_uniform;
		GLint modelview_matrix_uniform;

		GLint radius_uniform;

		GLint cam_pos_uniform;
		GLint cam_dir_uniform;

	public:
		static const int view_ray_attribute = 1;

		void Init(void);

		void SetKernel(int kernel_size, float *kernel);
		void SetNoiseTex(GLuint tex, CVector2 tex_scale);
		void SetTextures(GLuint depth, GLuint pos, GLuint normal);
		void SetMatrices(float *proj, float *modelview);
		void SetRadius(float radius);
		void SetCamera(CVector pos, CVector dir);
};


#endif
