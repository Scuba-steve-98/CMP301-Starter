#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class BlurVersShader : public BaseShader
{
public:
	BlurVersShader(ID3D11Device* device, HWND hwnd);
	~BlurVersShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float screenHeight, float focusNear, float focusFar, float blurNear, float blurFar); // whereever you are

private:
	struct ScreenBufferType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

	struct DepthBufferType
	{
		float focusNear;
		float focusFar;
		float blurNear;
		float blurFar;
	};

	void initShader(const wchar_t* vs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* depthSampleState;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* depthBlurBuffer;
};

