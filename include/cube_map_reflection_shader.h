
#ifndef _CUBE_MAP_REFLECTION_SHADER_H
#define _CUBE_MAP_REFLECTION_SHADER_H


class tCubeMapReflectionShader : public tShader
{
	private:
		GLint transformation_uniform;

		GLint cam_pos_uniform;

		GLint reflection_color_uniform;
		GLint cube_map_uniform;

		GLint normal_tex_uniform;

		int cube_map_tex_unit;

	public:
		void Init(tGBuffer *gbuffer);

		void SetCubeMapTexture(GLuint tex);
		void SetReflectionColor(tVector color);
		void SetCameraPosition(tVector pos);
		void SetTransformation(const float m[16]);

};


#endif
