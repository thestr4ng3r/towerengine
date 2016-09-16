
#ifndef _DEPTH_PASS_SHADER_H
#define _DEPTH_PASS_SHADER_H

class tDepthPassShader : public tFaceShader
{
private:
	GLint transformation_uniform;

	GLint diffuse_tex_enabled_uniform;
	GLint diffuse_tex_uniform;

	//GLint clip_uniform;
	//GLint clip_dist_uniform;

public:
	tDepthPassShader(void);

	//void SetClip(tVector c, float d);
	void SetBaseColorTexture(bool enabled, GLuint tex = 0);
	void SetTransformation(const float m[16]);
};


#endif
