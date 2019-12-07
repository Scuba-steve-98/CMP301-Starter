#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class HeightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuseColour;
		XMFLOAT4 ambientColour;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

public:
	HeightShader(ID3D11Device* device, HWND hwnd);
	~HeightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, ID3D11ShaderResourceView* heightmap, ID3D11ShaderResourceView* depthMap, const XMMATRIX& lightViewMatrix, const XMMATRIX& lightProjMatrix);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
};

