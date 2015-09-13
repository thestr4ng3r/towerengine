#ifndef _ENGINE_H
#define _ENGINE_H

class tEngine
{
	private:
		static bool arb_bindless_texture_supported;
		static bool arb_shading_language_420pack_supported;

	public:
		static void Init(void);
		static void Destroy(void);

		static bool GetARBBindlessTextureSupported(void)			{ return arb_bindless_texture_supported; }
		static bool GetARBShadingLanguage420PackSupported(void)		{ return arb_shading_language_420pack_supported; }
};

#endif
