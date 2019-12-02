// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#define WAVE_AMPLITUDE 0.148f
#define WAVE_FREQUENCY 2.634f
#define WAVE_SPEED 0.258f
#define PERSON_MOVE 622, 33, 500
#define PERSON_TORCH_SIZE 0.063, 0.063, 0.063
#define TORCH_MOVE 622, 69, 474
#define TORCH_ROTATE 4.71239
#define CAMP_MOVE 27.227, 1.223, 25.223
#define CAMP_SIZE 1.5, 1.5, 1.5

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
	textureMgr->loadTexture(L"person", L"res/person.png");
	textureMgr->loadTexture(L"torch", L"res/torch.png");
	textureMgr->loadTexture(L"campfire", L"res/campfire.jpg");

	// Initalise scene variables.
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	person = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/person.obj");
	torch = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/torchtris.obj");
	campfire = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/campfire.obj");

	waves = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waveShader = new WaveShader(renderer->getDevice(), hwnd);
	heightMap = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	heightShader = new HeightShader(renderer->getDevice(), hwnd);

	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	pointLight = new Light;

	spotLight = new Light;
	spotLight->setPosition(5, 4, 8);
	spotLight->setDiffuseColour(1, 1, 1, 1);
	spotLight->setDirection(1, 1, 1);
	spotLight->setSpecularPower(3);

	light = new Light;
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(0.7f, -0.7f, 0.0f);
	light->setPosition(0.f, 10.f, 0.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

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

	if (spotLight)
	{
		delete spotLight;
		spotLight = 0;
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


void App1::manipulation()
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
	waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED, shadowMap->getDepthMapSRV());
	waveShader->render(renderer->getDeviceContext(), waves->getIndexCount());

	worldMatrix = XMMatrixTranslation(0, -2.86, 0);
	heightMap->sendData(renderer->getDeviceContext());
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, textureMgr->getTexture(L"height"), shadowMap->getDepthMapSRV());
	heightShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	// Render models
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	person->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"person"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), person->getIndexCount());


	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	campfire->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"campfire"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	XMMATRIX rotate = XMMatrixRotationY(TORCH_ROTATE);
	worldMatrix = XMMatrixTranslation(TORCH_MOVE);
	worldMatrix = XMMatrixMultiply(rotate, worldMatrix);
	scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	torch->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"torch"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), torch->getIndexCount());

	//renderer->setZBuffer(false);
	//orthoMesh->sendData(renderer->getDeviceContext());
	//textureShader->setShaderParameters(renderer->getDeviceContext(), renderer->getWorldMatrix(), camera->getOrthoViewMatrix(), renderer->getOrthoMatrix(), shadowMap->getDepthMapSRV());
	//textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	//renderer->setZBuffer(true);

}

// Write a seperate depth pass for each manipulation

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render terrain
	waves->sendData(renderer->getDeviceContext());
	waveDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED, lightViewMatrix, lightProjectionMatrix);
	waveDepth->render(renderer->getDeviceContext(), waves->getIndexCount());

	worldMatrix = XMMatrixTranslation(0, -2.86, 0);

	heightMap->sendData(renderer->getDeviceContext());
	heightDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	heightDepth->render(renderer->getDeviceContext(), heightMap->getIndexCount());

	//worldMatrix = renderer->getWorldMatrix();
	//worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	//XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);


	// Render models
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	person->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), person->getIndexCount());


	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	XMMATRIX rotate = XMMatrixRotationY(TORCH_ROTATE);
	worldMatrix = XMMatrixTranslation(TORCH_MOVE);
	worldMatrix = XMMatrixMultiply(rotate, worldMatrix);
	scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	torch->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), torch->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::shadowPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render floor
	waves->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), waves->getIndexCount());


	worldMatrix = XMMatrixTranslation(0, -2.86, 0);

	heightMap->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());


	// Render models
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	person->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"person"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), person->getIndexCount());


	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	campfire->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"campfire"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	XMMATRIX rotate = XMMatrixRotationY(TORCH_ROTATE);
	worldMatrix = XMMatrixTranslation(TORCH_MOVE);
	worldMatrix = XMMatrixMultiply(rotate, worldMatrix);
	scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	torch->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"torch"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), torch->getIndexCount());
}


void App1::secondPass()
{

}


void App1::finalPass()
{

}


bool App1::render()
{


	depthPass();

	//shadowPass();
	manipulation();

	//Render GUI
	gui();

	//Present the rendered scene to the screen.
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

