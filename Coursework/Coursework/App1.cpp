// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"
#define WAVE_AMPLITUDE 0.148f
#define WAVE_FREQUENCY 2.634f
#define WAVE_SPEED 0.7f


#define BLUR_NEAR 0.861f
#define BLUR_FAR 0.999f
#define FOCUS_NEAR 0.936f
#define FOCUS_FAR 0.995f


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
	light = nullptr;
	heightMap = nullptr;
	heightShader = nullptr;
	orthoMesh = nullptr;
	shadowMap = nullptr;
	person = nullptr;
	torch = nullptr;
	campfire = nullptr;
	textureShader = nullptr;
	depthShader = nullptr;
	waveDepth = nullptr;
	heightDepth = nullptr;
	shadowShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	textureMgr->loadTexture(L"waves", L"res/water2.png");
	textureMgr->loadTexture(L"grass", L"res/grass2.png");
	textureMgr->loadTexture(L"height", L"res/height.png");
	textureMgr->loadTexture(L"person", L"res/person.png");
	textureMgr->loadTexture(L"torch", L"res/torch.png");
	textureMgr->loadTexture(L"campfire", L"res/campfire.jpg");

	// Initalise scene variables.
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	blur = new ShadowMap(renderer->getDevice(), screenWidth, screenHeight);

	person = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/person.obj");
	torch = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/torchtris.obj");
	campfire = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/campfire.obj");

	waves = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	waveShader = new WaveShader(renderer->getDevice(), hwnd);


	heightMap = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	heightShader = new HeightShader(renderer->getDevice(), hwnd);


	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	waveDepth = new WaveDepth(renderer->getDevice(), hwnd);
	heightDepth = new HeightDepth(renderer->getDevice(), hwnd);


	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);


	horizontal = new BlurHoorShader(renderer->getDevice(), hwnd);
	vertical = new BlurVersShader(renderer->getDevice(), hwnd);


	renderShadowTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	renderHorTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	renderVerTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	finalTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	light = new Light;
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setSpecularColour(1.0f, 0.9f, 0.9f, 1.0f);
	light->setSpecularPower(128.0f);
	light->setDirection(0.7f, -0.7f, 0.f);
	light->setPosition(-10.f, 20.f, 50.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	blurry = true;
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

	if (heightMap)
	{
		delete heightMap;
		heightMap = 0;
	}

	if (heightShader)
	{
		delete heightShader;
		heightShader = 0;
	}

	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = 0;
	}

	if (shadowMap)
	{
		delete shadowMap;
		shadowMap = 0;
	}

	if (person)
	{
		delete person;
		person = 0;
	}

	if (torch)
	{
		delete torch;
		torch = 0;
	}

	if (campfire)
	{
		delete campfire;
		campfire = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	if (waveDepth)
	{
		delete waveDepth;
		waveDepth = 0;
	}

	if (heightDepth)
	{
		delete heightDepth;
		heightDepth = 0;
	}

	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
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

	// Render terrain-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Render Waves
	worldMatrix = renderer->getWorldMatrix();
	waves->sendData(renderer->getDeviceContext());
	waveDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED);
	waveDepth->render(renderer->getDeviceContext(), waves->getIndexCount());

	// Render HeightMap
	worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f);
	heightMap->sendData(renderer->getDeviceContext());
	heightDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"grass"), light, textureMgr->getTexture(L"height"));
	heightDepth->render(renderer->getDeviceContext(), heightMap->getIndexCount());


	// Render models-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Render Person
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	person->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), person->getIndexCount());

	// Render Campfire
	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());

	// Render Torch
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
	renderShadowTex->setRenderTarget(renderer->getDeviceContext());
	renderShadowTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	light->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	
	// render terrain--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Render Heightmap
	worldMatrix = renderer->getWorldMatrix();
	waves->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED, shadowMap->getDepthMapSRV(), camera, lightViewMatrix, lightProjectionMatrix);
	waveShader->render(renderer->getDeviceContext(), waves->getIndexCount());

	// Render Waves
	worldMatrix = XMMatrixTranslation(0, -2.86, 0);
	heightMap->sendData(renderer->getDeviceContext());
	heightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, textureMgr->getTexture(L"height"), shadowMap->getDepthMapSRV(), lightViewMatrix, lightProjectionMatrix);
	heightShader->render(renderer->getDeviceContext(), heightMap->getIndexCount());


	// Render models----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Render Person
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	person->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"person"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), person->getIndexCount());

	// Render Torch
	XMMATRIX rotate = XMMatrixRotationY(TORCH_ROTATE);
	worldMatrix = XMMatrixTranslation(TORCH_MOVE);
	worldMatrix = XMMatrixMultiply(rotate, worldMatrix);
	scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	torch->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"torch"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), torch->getIndexCount());
	worldMatrix = renderer->getWorldMatrix();

	// Render Campfire
	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	campfire->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"campfire"), shadowMap->getDepthMapSRV(), light);
	shadowShader->render(renderer->getDeviceContext(), campfire->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


// Post Process ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App1::blurDepth()
{
	// Set the render target to be the render to texture.
	blur->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render terrain-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Render Waves
	worldMatrix = renderer->getWorldMatrix();
	waves->sendData(renderer->getDeviceContext());
	waveDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"waves"), light, run_time, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_SPEED);
	waveDepth->render(renderer->getDeviceContext(), waves->getIndexCount());

	// Render HeightMap
	worldMatrix = XMMatrixTranslation(0.f, -2.86, 0.f);
	heightMap->sendData(renderer->getDeviceContext());
	heightDepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), light, textureMgr->getTexture(L"height"));
	heightDepth->render(renderer->getDeviceContext(), heightMap->getIndexCount());


	// Render models-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Render Person
	worldMatrix = XMMatrixTranslation(PERSON_MOVE);
	XMMATRIX scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	person->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), person->getIndexCount());

	// Render Campfire
	worldMatrix = XMMatrixTranslation(CAMP_MOVE);
	scaleMatrix = XMMatrixScaling(CAMP_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	campfire->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), campfire->getIndexCount());

	// Render Torch
	XMMATRIX rotate = XMMatrixRotationY(TORCH_ROTATE);
	worldMatrix = XMMatrixTranslation(TORCH_MOVE);
	worldMatrix = XMMatrixMultiply(rotate, worldMatrix);
	scaleMatrix = XMMatrixScaling(PERSON_TORCH_SIZE);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	torch->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	depthShader->render(renderer->getDeviceContext(), torch->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::horBlurrification()
{
	float screenSizeX = (float)renderHorTex->getTextureWidth();

	renderHorTex->setRenderTarget(renderer->getDeviceContext());
	renderHorTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderHorTex->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontal->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderShadowTex->getShaderResourceView(), blur->getDepthMapSRV(), screenSizeX, FOCUS_NEAR, FOCUS_FAR, BLUR_NEAR, BLUR_FAR);
	horizontal->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::versBlurrification()
{
	float screenSizeY = (float)renderVerTex->getTextureHeight();

	renderVerTex->setRenderTarget(renderer->getDeviceContext());
	renderVerTex->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderVerTex->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	vertical->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, renderHorTex->getShaderResourceView(), blur->getDepthMapSRV(), screenSizeY, FOCUS_NEAR, FOCUS_FAR, BLUR_NEAR, BLUR_FAR);
	vertical->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}


void App1::finalPass()
{
	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	if (blurry)
		finalTex = renderVerTex;
	else
		finalTex = renderShadowTex;

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, finalTex->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//// Render GUI
	gui();
}


bool App1::render()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();

	run_time += timer->getTime();

	depthPass();

	shadowPass();

	if (blurry)
	{
		blurDepth();

		horBlurrification();

		versBlurrification();
	}


	finalPass();

	//// Present the rendered scene to the screen.
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
	ImGui::Checkbox("Activate Blur", &blurry);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
