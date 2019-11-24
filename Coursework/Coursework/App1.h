// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "HeightShader.h"
#include "WaveShader.h"
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void manipulation();
	void depthPass();
	void shadowPass();
	void secondPass();
	void finalPass();
	void gui();

private:
	HeightShader* heightShader;
	WaveShader* waveShader;
	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	ShadowMap* shadowMap;
	Light* light;
	Light* spotLight;
	Light* pointLight;

	PlaneMesh* heightMap;
	PlaneMesh* waves;
	CubeMesh* lamp;
	Model* person;
	Model* torch;
	Model* campfire;

	float amplitude, freq, speed, run_time;
	//float displace;
};

#endif