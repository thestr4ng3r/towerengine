
#include "towerengine.h"

CFaceShader *CEngine::face_shader = 0;

void CEngine::Init(void)
{
	int max_textures;

	printf("Supported OpenGL version: %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_textures);
	printf("Support of %d texture image units\n", max_textures);

	#ifdef _WIN32
		glewInit();
	#endif

	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	ilInit();
	iluInit();

	face_shader = new CFaceShader();
	face_shader->Init();
}

void CEngine::Destroy(void)
{
	delete face_shader;
}


void CEngine::BindFaceShader(void)
{
	face_shader->BindShader();
}


void CEngine::BindShader(GLhandleARB s)
{
	glUseProgramObjectARB(s);
}


void CEngine::UnbindShader(void)
{
	glUseProgramObjectARB(0);
}
