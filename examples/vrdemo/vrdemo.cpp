
#include <chrono>

#include <openvr.h>

#include <towerengine.h>
#include <vr_context_openvr.h>

#include <GLFW/glfw3.h>

#ifdef TOWERENGINE_ENABLE_IMGUI
#include <imgui.h>
#endif

#include "common.h"
#include "vrcontroller.h"
#include "vrplayer.h"


#define MAX_CONTROLLERS 2


const int screen_width = 1280;
const int screen_height = 720;


GLFWwindow *window = 0;

tVRContextOpenVR *vr_context = 0;

tWorld *world = 0;
tScene *scene = 0;

tVRForwardRenderer *renderer = 0;

tBulletDebugObject *bullet_debug_object = 0;

tMesh *small_cube_mesh = 0;
tMeshObject *small_cube_object = 0;

btCollisionShape *ground_shape = 0;
btDefaultMotionState *ground_motion_state = 0;
btRigidBody *ground_rigid_body = 0;


VRPlayer *vr_player = 0;
unsigned int controllers_count = 0;
VRController *vr_controllers[MAX_CONTROLLERS] = { 0, 0 };


void InitGLFW();
void InitEngine();
void InitScene();
void MainLoop();
void RenderGUI(float delta_time);
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

#ifdef TOWERENGINE_ENABLE_IMGUI
	tImGui::Init();
#endif

	vr_context = new tVRContextOpenVR(1.0f);
}

void InitScene()
{
	world = new tWorld();
	scene = new tScene(world);
	if(!scene->LoadFromFile("assets/vr.tes"))
	{
		fprintf(stderr, "Failed to load scene. Make sure to run this program in the examples/ directory.\n");
		Cleanup();
		exit(1);
	}
	scene->AddToWorld();

	for(tObject *object : world->GetObjects())
	{
		if(tPointLight *point_light = dynamic_cast<tPointLight *>(object))
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


	vr_player = new VRPlayer(tVec(0.0f, 0.0f, 0.0f));

	for(unsigned int i = 0; i<MAX_CONTROLLERS; i++)
		vr_controllers[i] = new VRController(world, vr_player);

	//tCoordinateSystemObject *coo = new tCoordinateSystemObject();
	//world->AddObject(coo);

	bullet_debug_object = new tBulletDebugObject();
	world->AddObject(bullet_debug_object);
	bullet_debug_object->SetEnabled(false);


	small_cube_mesh = new tMesh("assets/meshes/SmallCube.tem");
	small_cube_object = new tMeshObject(small_cube_mesh);
	tTransform t = small_cube_object->GetTransform();
	t.SetPosition(tVec(0.0, 1.0, 0.0));
	small_cube_object->SetTransform(t);
	small_cube_object->InitBoxRigidBody(tVec(0.1f, 0.1f, 0.1f), 1.0f);
	world->AddObject(small_cube_object);
	world->GetDynamicsWorld()->removeRigidBody(small_cube_object->GetRigidBody());
	world->GetDynamicsWorld()->addRigidBody(small_cube_object->GetRigidBody(),
											btBroadphaseProxy::DefaultFilter | MASK_PICKABLE,
											btBroadphaseProxy::AllFilter);
	small_cube_object->GetRigidBody()->setUserPointer(small_cube_object->GetRigidBody());
	

	ground_shape = new btBoxShape(btVector3(10.0f, 0.5f, 10.0f)); 
	ground_motion_state = new btDefaultMotionState(btTransform(btMatrix3x3::getIdentity(), btVector3(0.0f, -0.5f, 0.0f)));
	ground_rigid_body = new btRigidBody(0.0f, ground_motion_state, ground_shape);
	world->GetDynamicsWorld()->addRigidBody(ground_rigid_body,
											btBroadphaseProxy::DefaultFilter | MASK_TELEPORT,
											btBroadphaseProxy::AllFilter);
}


float delta_time = 1.0f / 90.0f;

void UpdateControllers();
void RenderGUI();

void MainLoop()
{
	while(!glfwWindowShouldClose(window))
	{
		auto start_time = std::chrono::high_resolution_clock::now();

		tVector center_pos;
		tVector center_dir;
		vr_context->StartFrame(tVec(0.0f, 0.0f), vr_player->GetOrigin(), center_pos, center_dir, renderer->GetCameras());
		vr_player->UpdatePosition(center_pos, center_dir);

		UpdateControllers();


		vr_player->Update();
		for (unsigned int i = 0; i < controllers_count; i++)
			vr_controllers[i]->Update();


		world->Step(delta_time, 8, 1.0f / 240.0f);

		renderer->Render(vr_context->GetFBO());
		vr_context->FinishFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		vr_context->BlitMirrorFrame(0, 0, screen_width, screen_height);

		RenderGUI();
		glfwSwapBuffers(window);

		glfwPollEvents();

		auto end_time = std::chrono::high_resolution_clock::now();
		auto delta_time_duration = std::chrono::duration_cast<std::chrono::duration<float>>(end_time - start_time);
		delta_time = delta_time_duration.count();
	}
}


void UpdateControllers()
{
	vr::TrackedDeviceIndex_t controller_indexes[MAX_CONTROLLERS];
	vr::TrackedDevicePose_t controller_poses[MAX_CONTROLLERS];
	vr::VRControllerState_t controller_states[MAX_CONTROLLERS];
	tMesh *controller_mesh[MAX_CONTROLLERS];
	controllers_count = vr_context->GetOpenVRControllerStates(vr_player->GetOrigin(),
										  MAX_CONTROLLERS,
										  controller_indexes,
										  controller_poses,
										  controller_states,
										  controller_mesh);

	for(unsigned int i = 0; i < controllers_count; i++)
		vr_controllers[i]->UpdateController(controller_indexes[i], controller_poses[i], controller_states[i], controller_mesh[i]);
}

void RenderGUI()
{
#ifdef TOWERENGINE_ENABLE_IMGUI
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	tImGui::BeginFrame(delta_time, screen_width, screen_height);
	ImGui::Render();
#endif
}


void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Cleanup()
{
	delete renderer;
	delete scene;
	delete world;

	delete ground_rigid_body;
	delete ground_motion_state;
	delete ground_shape;

	delete small_cube_object;
	delete small_cube_mesh;

	delete bullet_debug_object;

	for(unsigned int i = 0; i < controllers_count; i++)
		delete vr_controllers[i];

#ifdef TOWERENGINE_ENABLE_IMGUI
	tImGui::Shutdown();
#endif

	delete vr_context;
	glfwTerminate();
}