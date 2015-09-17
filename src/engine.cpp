
#include "towerengine.h"

#include <IL/il.h>
#include <IL/ilu.h>


bool tEngine::arb_bindless_texture_supported = false;
//bool tEngine::arb_shading_language_420pack_supported = false;

void tEngine::Init(void)
{
	int v;
	printf("Running with OpenGL version: %s\n", glGetString(GL_VERSION));
	//glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &v);
	//printf("Support of %d combined texture image units\n", v);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &v);
	printf("Support of %d color attachments\n", v);

	glewExperimental = GL_TRUE;
	GLenum glew_r = glewInit();
	if(glew_r != GLEW_OK)
	{
		printf("Failed to initialize glew: %s\n", glewGetErrorString(glew_r));
		return;
	}


	int ext_count;
	glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

	if(GLEW_ARB_bindless_texture)
		arb_bindless_texture_supported = true;

	if(!arb_bindless_texture_supported)
		printf("ARB_bindless_texture is not supported by the graphics card or driver! This might have a negative impact on performance.\n");


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

