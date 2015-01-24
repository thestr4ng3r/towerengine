
#include "towerengine.h"

tGeometryPassShader *tEngine::geometry_pass_shader = 0;
tAmbientLightingShader *tEngine::ambient_lighting_shader = 0;
tDirectionalLightingShader *tEngine::directional_lighting_shader = 0;
tPointLightingShader *tEngine::point_lighting_shader = 0;
tSSAOLightingShader *tEngine::ssao_lighting_shader = 0;

tSkyBoxShader *tEngine::skybox_shader = 0;
tPointShadowShader *tEngine::point_shadow_shader = 0;
tPointShadowBlurShader *tEngine::point_shadow_blur_shader = 0;
tDirectionalShadowShader *tEngine::directional_shadow_shader = 0;
tDirectionalShadowBlurShader *tEngine::directional_shadow_blur_shader = 0;
tColorShader *tEngine::color_shader = 0;
tFaceShader *tEngine::current_face_shader = 0;
tPostProcessShader *tEngine::post_process_shader = 0;
tSSAOShader *tEngine::ssao_shader = 0;

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

	ilInit();
	iluInit();

	geometry_pass_shader = new tGeometryPassShader();
	geometry_pass_shader->Init();
	SetCurrentFaceShader(geometry_pass_shader);

	directional_lighting_shader = new tDirectionalLightingShader();
	directional_lighting_shader->Init();

	point_lighting_shader = new tPointLightingShader();
	point_lighting_shader->Init();

	ambient_lighting_shader = new tAmbientLightingShader();
	ambient_lighting_shader->Init();

	ssao_lighting_shader = new tSSAOLightingShader();
	ssao_lighting_shader->Init();

	skybox_shader = new tSkyBoxShader();
	skybox_shader->Init();

	point_shadow_shader = new tPointShadowShader();
	point_shadow_shader->Init();

	point_shadow_blur_shader = new tPointShadowBlurShader();
	point_shadow_blur_shader->Init();

	directional_shadow_shader = new tDirectionalShadowShader();
	directional_shadow_shader->Init();

	directional_shadow_blur_shader = new tDirectionalShadowBlurShader();
	directional_shadow_blur_shader->Init();

	color_shader = new tColorShader();
	color_shader->Init();

	post_process_shader = new tPostProcessShader();
	post_process_shader->Init();

	ssao_shader = new tSSAOShader();
	ssao_shader->Init();
}

void tEngine::Destroy(void)
{
	delete geometry_pass_shader;
	delete skybox_shader;
	delete point_shadow_shader;
	delete point_shadow_blur_shader;
	delete directional_shadow_shader;
	delete color_shader;
}

