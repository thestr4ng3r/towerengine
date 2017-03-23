
#ifndef _POINT_LIGHT_SHADOW_H
#define _POINT_LIGHT_SHADOW_H


class tPointLightShadow
{
	private:
		tPointLight *light;

		tRenderObjectSpace *render_object_space;

		GLuint tex;
		GLuint fbo;
		GLuint depth_rbo;

		bool blur_enabled;
		float blur_size;
		GLuint blur_tex;
		GLuint blur_fbo;
		static const GLenum blur_draw_buffers[6];

		tVAO *blur_vao;
		tVBO<float> *blur_vertex_vbo;

		int size;

		//bool culled;

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		GLuint64 tex_handle;
		bool tex_resident;
#endif

	public:
		tPointLightShadow(tPointLight *light, int size, bool blur_enabled = false);
		~tPointLightShadow(void);

		void Render(tDeferredRenderer *renderer);

		GLuint GetShadowMap(void)			{ return tex; }

#ifndef TOWERENGINE_DISABLE_BINDLESS_TEXTURE
		GLuint64 GetTextureHandle(void)		{ return tex_handle; }
		void MakeTextureHandleResident(bool resident);
#endif

		void SetBlurEnabled(bool enabled)	{ blur_enabled = enabled; }
		void SetBlurSize(float size)		{ blur_size = size; }
		//void SetCulled(bool culled)			{ this->culled = culled; }

		bool GetBlurEnabled(void)			{ return blur_enabled; }
		float GetBlurSize(void)				{ return blur_size; }
		tRenderObjectSpace *GetRenderObjectSpace(void)	{ return render_object_space; }
		//bool GetCulled(void)				{ return culled; }
};



#endif
