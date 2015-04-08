
#include "towerengine.h"


const float tEngine::identity_matrix4[16] = { 1.0, 0.0, 0.0, 0.0,
										  	  0.0, 1.0, 0.0, 0.0,
										  	  0.0, 0.0, 1.0, 0.0,
										  	  0.0, 0.0, 0.0, 1.0 };

void tEngine::Init(void)
{
	int max_textures;

	printf("Supported OpenGL version: %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures);
	printf("Support of %d combined texture image units\n", max_textures);

	#ifdef _WIN32
		GLenum glew_r = glewInit();
		if(glew_r != GLEW_OK)
		{
			printf("Failed to initialize glew: %s\n", glewGetErrorString(glew_r));
			return;
		}
	#endif

	srand(time(0));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	ilInit();
	iluInit();

}

void tEngine::Destroy(void)
{
}

