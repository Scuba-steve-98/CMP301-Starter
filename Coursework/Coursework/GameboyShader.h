#pragma once
#include"DXF.h"


class GameboyShader : public BaseShader
{
private:
	struct TimeBufferType
	{
		float time;
		XMFLOAT2 res;
		float padding;
	};

public:
	GameboyShader(ID3D11Device* device, HWND hwnd);
	~GameboyShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float x, float y, float deltaTime);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* timeBuffer;

	float timer;
	XMFLOAT2 res;
};

