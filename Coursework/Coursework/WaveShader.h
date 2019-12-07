#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;


class WaveShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};


	struct LightBufferType
	{
		XMFLOAT4 diffuseColour;
		XMFLOAT4 ambientColour;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColour;
	};

	struct CamBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct TimeBufferType
	{
		float time;
		float amplitude;
		float speed;
		float frequency;

	};

public:
	WaveShader(ID3D11Device* device, HWND hwnd);
	~WaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float deltaTime, float amplitude, float freq, float speed, ID3D11ShaderResourceView* depthMap, FPCamera* camera, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* camBuffer;

	float amplitude, speed, frequency;
};

