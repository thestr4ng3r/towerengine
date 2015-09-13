#ifndef _ENGINE_H
#define _ENGINE_H

class tEngine
{
	private:
		static bool arb_bindless_texture_supported;

	public:
		static void Init(void);
		static void Destroy(void);

		static bool GetARBBindlessTextureSupported(void)	{ return arb_bindless_texture_supported; }
};

#endif
