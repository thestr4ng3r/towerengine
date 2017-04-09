
#include <towerengine.h>
#include <vr_context_openvr.h>

#include <GLFW/glfw3.h>


const int screen_width = 1280;
const int screen_height = 720;


GLFWwindow *window = 0;

tVRContext *vr_context = 0;

tWorld *world = 0;
tScene *scene = 0;

tVRForwardRenderer *renderer;


void InitGLFW();
void InitEngine();
void InitScene();
void MainLoop();
void Cleanup();

int main()
{
	InitGLFW();
	InitEngine();
	InitScene();
	MainLoop();
	Cleanup();
	return 0;
}

void InitGLFW()
{
	if (!glfwInit())
		exit(1);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(screen_width, screen_height, "Simple TowerEngine Demo", 0, 0);
	if (!window)
	{
		Cleanup();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // do not limit VR refresh rate to monitor
}

void InitEngine()
{
	if (!tEngine::Init())
	{
		Cleanup();
		exit(1);
	}

	vr_context = new tVRContextOpenVR(1.0f);
}

void InitScene()
{
	world = new tWorld();
	scene = new tScene(world);
	if (!scene->LoadFromFile("assets/vr.tes"))
	{
		fprintf(stderr, "Failed to load scene. Make sure to run this program in the examples/.\n");
		Cleanup();
		exit(1);
	}
	scene->AddToWorld();

	for (tObject *object : world->GetObjects())
	{
		if (tPointLight *point_light = dynamic_cast<tPointLight *>(object))
		{
			point_light->InitShadow(256, true);
		}
	}

	renderer = new tVRForwardRenderer(vr_context->GetEyeRenderWidth(tVRContext::LEFT),
		vr_context->GetEyeRenderHeight(tVRContext::LEFT),
		vr_context->GetEyeRenderWidth(tVRContext::RIGHT),
		vr_context->GetEyeRenderHeight(tVRContext::RIGHT),
		4,
		world);
}

void MainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		tVector center_pos;
		tVector center_dir;
		vr_context->StartFrame(tVec(0.0f, 0.0f), tVec(0.0f, 0.0f, 0.0f), center_pos, center_dir, renderer->GetCameras());
		renderer->Render(vr_context->GetFBO());
		vr_context->FinishFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		vr_context->BlitMirrorFrame(0, 0, screen_width, screen_height);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void Cleanup()
{
	delete renderer;
	delete scene;
	delete world;
	delete vr_context;
	glfwTerminate();
}