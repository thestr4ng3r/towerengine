
#include "towerengine.h"

#include <IL/il.h>
#include <IL/ilu.h>



void tEngine::Init(void)
{
	int v;
	printf("Supported OpenGL version: %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &v);
	printf("Support of %d combined texture image units\n", v);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v);
	printf("Support of %d color attachments\n", v);

	#ifdef _WIN32
		glewExperimental = GL_TRUE;
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

