
#include <towerengine.h>
#include <GLFW/glfw3.h>

const int screen_width = 640;
const int screen_height = 480;

enum ControlKey { UP = 0, DOWN, LEFT, RIGHT, FORWARD, BACKWARD, COUNT };
bool control_keys[ControlKey::COUNT];

bool deferred = false;
bool deferred_change = false;


tWorld *world;

tRenderer *renderer = 0;
tCamera *camera;


void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void InitRenderer(bool deferred);

int main()
{
	GLFWwindow *window;

	if(!glfwInit())
		return 1;

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(screen_width, screen_height, "TowerEngine Cube Demo (Press F1 to switch between Forward and Deferred)", 0, 0);
	if(!window)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	memset(control_keys, sizeof(control_keys), 0);
	glfwSetKeyCallback(window, KeyCallback);

	if(!tEngine::Init())
	{
		glfwTerminate();
		return 1;
	}

	world = new tWorld();
	tScene *scene = new tScene(world);
	if(!scene->LoadFromFile("assets/simple.tes"))
	{
		fprintf(stderr, "Failed to load scene. Make sure to run this program in the examples/.\n");
		delete scene;
		delete world;
		glfwTerminate();
		return 1;
	}
	scene->AddToWorld();

	for(tObject *object : world->GetObjects())
	{
		if(tPointLight *point_light = dynamic_cast<tPointLight *>(object))
		{
			point_light->InitShadow(256, true);
		}
	}

	InitRenderer(deferred);
	printf("Switch between Deferred and Forward by pressing F1.\n");

	tVector2 cam_rot = tVec(0.5f, -0.7f);
	float cam_dist = 5.0f;

	float delta_time = 0.0f;
	while(!glfwWindowShouldClose(window))
	{
		if(deferred_change != deferred)
		{
			deferred = deferred_change;
			InitRenderer(deferred);
		}

		double start_time = glfwGetTime();

		cam_rot.x += ((control_keys[LEFT] ? -1.0f : 0.0f) + (control_keys[RIGHT] ? 1.0f : 0.0f)) * 2.0f * delta_time;
		cam_rot.y += ((control_keys[UP] ? -1.0f : 0.0f) + (control_keys[DOWN] ? 1.0f : 0.0f)) * 2.0f * delta_time;
		cam_dist += ((control_keys[FORWARD] ? -1.0f : 0.0f) + (control_keys[BACKWARD] ? 1.0f : 0.0f)) * 5.0f * delta_time;
		if(cam_dist < 0.1f)
			cam_dist = 0.1f;

		tVector cam_pos = (tMatrix3::GetEuler(tVec(0.0f, cam_rot.x, 0.0f))
						   * tMatrix3::GetEuler(tVec(cam_rot.y, 0.0f, 0.0f)))
						  * tVec(0.0f, 0.0f, cam_dist);
		camera->SetPosition(cam_pos);
		camera->SetDirection((-cam_pos).Normalized());

		if(deferred)
			static_cast<tDefaultDeferredRenderer *>(renderer)->Render(0, 0, 0, screen_width, screen_height);
		else
			static_cast<tDefaultForwardRenderer *>(renderer)->Render(0, 0, 0, screen_width, screen_height);

		glfwSwapBuffers(window);
		glfwPollEvents();

		double end_time = glfwGetTime();
		delta_time = (float)(end_time - start_time);
	}

	glfwTerminate();
	delete renderer;
	delete scene;
	delete world;
	return 0;
}

void InitRenderer(bool deferred)
{
	delete renderer;

	if(deferred)
	{
		renderer = new tDefaultDeferredRenderer(screen_width, screen_height, world);
		camera = static_cast<tDefaultDeferredRenderer *>(renderer)->GetCamera();
	}
	else
	{
		renderer = new tDefaultForwardRenderer(world);
		camera = static_cast<tDefaultForwardRenderer *>(renderer)->GetCamera();
	}

	camera->SetFOVVerticalAngle(60.0f, (float)screen_width / (float)screen_height);

	if(deferred)
		printf("Initialized Deferred Renderer.\n");
	else
		printf("Initialized Forward Renderer.\n");
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		switch(key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;

			case GLFW_KEY_F1:
				deferred_change = !deferred;
				break;

			case GLFW_KEY_UP:
				control_keys[UP] = true;
				break;
			case GLFW_KEY_DOWN:
				control_keys[DOWN] = true;
				break;
			case GLFW_KEY_LEFT:
				control_keys[LEFT] = true;
				break;
			case GLFW_KEY_RIGHT:
				control_keys[RIGHT] = true;
				break;
			case GLFW_KEY_A:
				control_keys[FORWARD] = true;
				break;
			case GLFW_KEY_Z:
			case GLFW_KEY_Y:
				control_keys[BACKWARD] = true;
				break;
			default:
				break;
		}
	}
	else if(action == GLFW_RELEASE)
	{
		switch(key)
		{
			case GLFW_KEY_UP:
				control_keys[UP] = false;
				break;
			case GLFW_KEY_DOWN:
				control_keys[DOWN] = false;
				break;
			case GLFW_KEY_LEFT:
				control_keys[LEFT] = false;
				break;
			case GLFW_KEY_RIGHT:
				control_keys[RIGHT] = false;
				break;
			case GLFW_KEY_A:
				control_keys[FORWARD] = false;
				break;
			case GLFW_KEY_Z:
			case GLFW_KEY_Y:
				control_keys[BACKWARD] = false;
				break;
			default:
				break;
		}
	}
}