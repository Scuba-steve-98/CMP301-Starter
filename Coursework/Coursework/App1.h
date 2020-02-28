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
#include "WaveDepth.h"
#include "HeightDepth.h"
#include "BlurHoorShader.h"
#include "BlurVersShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void shadowPass();
	void finalPass();
	void gui();

	void blurDepth();
	void horBlurrification();
	void versBlurrification();

private:
	HeightShader* heightShader;
	WaveShader* waveShader;
	TextureShader* textureShader;
	ShadowShader* shadowShader;
	DepthShader* depthShader;
	ShadowMap* shadowMap;
	ShadowMap* blur;
	HeightDepth* heightDepth;

	WaveDepth* waveDepth;
	BlurHoorShader* horizontal;
	BlurVersShader* vertical;
	Light* light;

	RenderTexture* renderDepthTex;
	RenderTexture* renderShadowTex;
	RenderTexture* renderHorTex;
	RenderTexture* renderVerTex;
	RenderTexture* finalTex;

	PlaneMesh* heightMap;
	PlaneMesh* waves;
	CubeMesh* lamp;
	Model* person;
	Model* torch;
	Model* campfire;

	float run_time;
	bool blurry;

	OrthoMesh* orthoMesh;
};

#endif