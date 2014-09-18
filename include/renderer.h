
#ifndef _RENDERER_H
#define _RENDERER_H


class CRenderer
{
	private:
		CWorld *world;

		CGBuffer *gbuffer;
		int screen_width, screen_height;

		GLuint fbo;
		GLuint color_tex;
		GLuint depth_rbo;

		int point_light_count;
		float *point_light_pos, *point_light_color, *point_light_distance;
		int *point_light_shadow_enabled;
		GLuint *point_light_shadow_maps;

		int dir_light_count;
		float *dir_light_dir, *dir_light_color, *dir_light_shadow_clip, *dir_light_shadow_tex_matrix;
		float *dir_light_shadow_splits_count, *dir_light_shadow_splits_z;
		int *dir_light_shadow_enabled;
		GLuint *dir_light_shadow_maps;

		void GeometryPass(void);
		void LightPass(void);

	public:
		CRenderer(int width, int height, CWorld *world = 0);
		~CRenderer();

		void SetWorld(CWorld *world)	{ this->world = world; }

		void Render(void);
};


#endif
