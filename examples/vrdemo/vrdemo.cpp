
#include <openvr.h>

#include <towerengine.h>
#include <vr_context_openvr.h>

#include <GLFW/glfw3.h>


const int screen_width = 1280;
const int screen_height = 720;


GLFWwindow *window = 0;

tVRContextOpenVR *vr_context = 0;

tWorld *world = 0;
tScene *scene = 0;

tMeshObject *controller_mesh_object[2];

tVRForwardRenderer *renderer;


void InitGLFW();
void InitEngine();
void InitScene();
void MainLoop();
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
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
	window = glfwCreateWindow(screen_width, screen_height, "TowerEngine VR Demo", 0, 0);
	if (!window)
	{
		Cleanup();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // do not limit VR refresh rate to monitor

	glfwSetKeyCallback(window, KeyCallback);
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

	for (int i = 0; i<2; i++)
	{
		controller_mesh_object[i] = new tMeshObject(0);
		world->AddObject(controller_mesh_object[i]);
	}
}

void MainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		tVector origin = tVec(0.0f, 0.0f, 0.0f);

		tVector center_pos;
		tVector center_dir;
		vr_context->StartFrame(tVec(0.0f, 0.0f), origin, center_pos, center_dir, renderer->GetCameras());

		vr::TrackedDevicePose_t controller_poses[2];
		vr::VRControllerState_t controller_states[2];
		tMesh *controller_mesh[2];
		vr_context->GetOpenVRControllerStates(origin, controller_poses, controller_states, controller_mesh);

		for (int i = 0; i < 2; i++)
		{
			tMatrix4 mat = ConvertOpenVRMatrix(controller_poses[i].mDeviceToAbsoluteTracking);
			float *mat_v = mat.GetData();

			tMatrix3 basis = tMatrix3(tVec(mat_v[0], mat_v[1], mat_v[2]),
				tVec(mat_v[4], mat_v[5], mat_v[6]),
				tVec(mat_v[8], mat_v[9], mat_v[10]));

			tVector pos = origin + tVec(mat_v[3], mat_v[7], mat_v[11]);

			controller_mesh_object[i]->SetTransform(tTransform(basis, pos));
			controller_mesh_object[i]->SetMesh(controller_mesh[i]);
		}

		//vr_last_buttons_pressed = vr_current_buttons_pressed;
		//vr_current_buttons_pressed = controller_states[0].ulButtonPressed | controller_states[1].ulButtonPressed;

		//show_crosshair = controller_states[0].rAxis[1].x > 0.1 || controller_states[1].rAxis[1].x > 0.1;


		renderer->Render(vr_context->GetFBO());
		vr_context->FinishFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		vr_context->BlitMirrorFrame(0, 0, screen_width, screen_height);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
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