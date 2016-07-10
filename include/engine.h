#ifndef _ENGINE_H
#define _ENGINE_H

class tEngine
{
	private:
		static bool arb_bindless_texture_supported;
		static bool arb_shading_language_include_supported;

	public:
		static bool Init(std::string *error = 0);

		static bool GetARBBindlessTextureSupported(void)				{ return arb_bindless_texture_supported; }
		static bool GetARBShadingLanguageIncludeSupported(void)			{ return arb_shading_language_include_supported; }
};

#endif
