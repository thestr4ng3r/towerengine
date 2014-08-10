
#include "towerengine.h"

CDefaultFaceShader *CEngine::default_face_shader = 0;
CSkyBoxShader *CEngine::skybox_shader = 0;
CPointShadowShader *CEngine::point_shadow_shader = 0;
CPointShadowBlurShader *CEngine::point_shadow_blur_shader = 0;
CDirectionalShadowShader *CEngine::directional_shadow_shader = 0;
CFaceShader *CEngine::current_face_shader = 0;

const float CEngine::identity_matrix4[16] = { 1.0, 0.0, 0.0, 0.0,
										  0.0, 1.0, 0.0, 0.0,
										  0.0, 0.0, 1.0, 0.0,
										  0.0, 0.0, 0.0, 1.0 };

void CEngine::Init(void)
{
	int max_textures;

	printf("Supported OpenGL version: %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures);
	printf("Support of %d combined texture image units\n", max_textures);

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

	default_face_shader = new CDefaultFaceShader();
	default_face_shader->Init();
	SetCurrentFaceShader(default_face_shader);

	skybox_shader = new CSkyBoxShader();
	skybox_shader->Init();

	point_shadow_shader = new CPointShadowShader();
	point_shadow_shader->Init();

	point_shadow_blur_shader = new CPointShadowBlurShader();
	point_shadow_blur_shader->Init();

	directional_shadow_shader = new CDirectionalShadowShader();
	directional_shadow_shader->Init();
}

void CEngine::Destroy(void)
{
	delete default_face_shader;
	delete skybox_shader;
	delete point_shadow_shader;
	delete point_shadow_blur_shader;
	delete directional_shadow_shader;
}

