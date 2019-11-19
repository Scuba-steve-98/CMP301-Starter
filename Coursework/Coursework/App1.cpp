// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#define WAVE_AMPLITUDE 0.148f
#define WAVE_FREQUENCY 2.634f
#define WAVE_SPEED 0.258f

App1::App1()
{
	waves = nullptr;
	waveShader = nullptr;
	heightMap = nullptr;
	heightShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture(L"waves", L"res/water2.png");
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"height", L"res/height.png");

	// Initalise scene variables.
	waves = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waveShader = new WaveShader(renderer->getDevice(), hwnd);
	heightMap = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	heightShader = new HeightShader(renderer->getDevice(), hwnd);
	light = new Light;
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(0.7f, -0.7f, 0.0f);
	run_time = 0.f;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (waves)
	{
		delete waves;
		waves = 0;
	}

	if (waveShader)
	{
		delete waveShader;
		waveShader = 0;
	}

	if (light)
	{
		delete light;
		light = 0;
	}

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}


void App1::depthPass()
{

}


void App1::secondPass()
{

}


void App1::finalPass()
{

}


bool App1::render()
{
	// Calculate run time for manipulation
	run_time += timer->getTime();

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	waves->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED);
	waveShader->render(renderer->getDeviceContext(), waves->getIndexCount());

	heightMap->sendData(renderer->getDeviceContext());
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, textureMgr->getTexture(L"height"));
	heightShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());


	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

