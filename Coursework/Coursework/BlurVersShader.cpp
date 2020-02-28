#include "BlurVersShader.h"


BlurVersShader::BlurVersShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"blurVers_vs.cso", L"blurVer_ps.cso");
}


BlurVersShader::~BlurVersShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the depth sampler state.
	if (depthSampleState)
	{
		depthSampleState->Release();
		depthSampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release screen size constant buffer
	if (screenSizeBuffer)
	{
		screenSizeBuffer->Release();
		screenSizeBuffer = 0;
	}

	// Release depth blur constant buffer
	if (depthBlurBuffer)
	{
		depthBlurBuffer->Release();
		depthBlurBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void BlurVersShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC screenBufferDesc;
	D3D11_BUFFER_DESC depthBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for depth sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	renderer->CreateSamplerState(&samplerDesc, &depthSampleState);

	// Setup screen size buffer
	screenBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenBufferDesc.ByteWidth = sizeof(ScreenBufferType);
	screenBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenBufferDesc.MiscFlags = 0;
	screenBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&screenBufferDesc, NULL, &screenSizeBuffer);

	// Setup depth blur buffer
	depthBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	depthBufferDesc.ByteWidth = sizeof(DepthBufferType);
	depthBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	depthBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&depthBufferDesc, NULL, &depthBlurBuffer);
}


void BlurVersShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float screenHeight, float focusNear, float focusFar, float blurNear, float blurFar)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;


	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &depthSampleState);

	// Send screen width to pixel shader
	ScreenBufferType* screenSizePtr;
	deviceContext->Map(screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	screenSizePtr = (ScreenBufferType*)mappedResource.pData;
	screenSizePtr->screenHeight = screenHeight;
	screenSizePtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(screenSizeBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &screenSizeBuffer);

	// Send screen width to pixel shader
	DepthBufferType* depthBlurPtr;
	deviceContext->Map(depthBlurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	depthBlurPtr = (DepthBufferType*)mappedResource.pData;
	depthBlurPtr->focusNear = focusNear;
	depthBlurPtr->focusFar = focusFar;
	depthBlurPtr->blurNear = blurNear;
	depthBlurPtr->blurFar = blurFar;
	deviceContext->Unmap(depthBlurBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &depthBlurBuffer);
}
