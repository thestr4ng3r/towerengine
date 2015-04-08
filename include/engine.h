#ifndef _ENGINE_H
#define _ENGINE_H

class tEngine
{
	public:
		static const float identity_matrix4[16];

		static void Init(void);
		static void Destroy(void);
};

#endif
