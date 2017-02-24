
#include "towerengine.h"


#ifdef TOWERENGINE_BUILD_DEVIL
#include <IL/il.h>
#include <IL/ilu.h>
#endif


bool tEngine::arb_bindless_texture_supported = false;
bool tEngine::arb_shading_language_include_supported = false;

tNoOpShader *tEngine::no_op_shader = 0;


const char *glew_init_error_msg = "Failed to initialize GLEW.";
const char *arb_bindless_texture_error_msg = "ARB_bindless_texture is not supported by the graphics card or driver. This might have a negative impact on performance.";
const char *arb_shading_language_include_error_msg = "ARB_shading_language_include is not supported by the graphics card or driver.";

bool tEngine::Init(std::string *error)
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
		*error = std::string("Failed to initialize GLEW: ") + glew_init_error_msg;
		fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glew_r));
		return false;
	}


	int ext_count;
	glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

	if(GLEW_ARB_bindless_texture)
		arb_bindless_texture_supported = true;

	if(GLEW_ARB_shading_language_include)
		arb_shading_language_include_supported = true;



	if(!arb_bindless_texture_supported)
		fprintf(stderr, "%s\n", arb_bindless_texture_error_msg);

	if(!arb_shading_language_include_supported)
	{
		*error = std::string(arb_shading_language_include_error_msg);
		fprintf(stderr, "%s\n", arb_shading_language_include_error_msg);
		return false;
	}

	tShader::InitSource();


	srand(time(0));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#ifdef TOWERENGINE_BUILD_DEVIL
	ilInit();
	iluInit();
#endif

	no_op_shader = new tNoOpShader();

	return true;
}


void tEngine::Shutdown(void)
{
	delete no_op_shader;
}