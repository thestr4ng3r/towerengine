
#include <towerengine.h>
#include <GLFW/glfw3.h>

const int screen_width = 640;
const int screen_height = 480;

int main()
{
	GLFWwindow *window;

	if(!glfwInit())
		return 1;

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(screen_width, screen_height, "Simple TowerEngine Demo", 0, 0);
	if(!window)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	if(!tEngine::Init())
	{
		glfwTerminate();
		return 1;
	}

	tWorld *world = new tWorld();
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

	for(int i=0; i<world->GetPointLightsCount(); i++)
		world->GetPointLight(i)->InitShadow(256, true);

	tDefaultForwardRenderer *renderer = new tDefaultForwardRenderer(world);

	tCamera *camera = renderer->GetCamera();
	camera->SetFOVVerticalAngle(60.0f, (float)screen_width / (float)screen_height);
	camera->SetPosition(tVec(4.0, 4.0, 4.0));
	camera->SetDirection(tVec(-1.0, -0.7, -1.0).Normalized());

	while(!glfwWindowShouldClose(window))
	{
		renderer->Render(0, 0, 0, screen_width, screen_height);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}