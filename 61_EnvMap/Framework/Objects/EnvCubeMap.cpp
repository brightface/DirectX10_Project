#include "Framework.h"
#include "EnvCubeMap.h"

EnvCubeMap::EnvCubeMap(Shader * shader, float width, float height)
	: shader(shader), position(0, 0, 0), width(width), height(height)
{
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Create Texture2D - RTV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6;
		desc.Format = rtvFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &rtvTexture));
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv));
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv));
	}


	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//Create Texture - DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6;
		desc.Format = dsvFormat;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &dsvTexture));
	}

	//CreateDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv));
	}


	viewport = new Viewport((float)width, (float)height);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_EnvCube");

	sSrv = shader->AsSRV("EnvCubeMap");
}

EnvCubeMap::~EnvCubeMap()
{
	SafeRelease(rtvTexture);
	SafeRelease(srv);
	SafeRelease(rtv);

	SafeRelease(dsvTexture);
	SafeRelease(dsv);

	SafeDelete(viewport);
	SafeDelete(buffer);
}

void EnvCubeMap::PreRender(Vector3 & position, Vector3 & scale, float zNear, float zFar, float fov)
{
	this->position = position;
	//이게 핵심이야 6개.
	//Create ViewMatrix
	{
		float x = this->position.x;
		float y = this->position.y;
		float z = this->position.z;

		//임시 구조체 정의가능
		struct LookAt
		{
			Vector3 LookAt; //카메라 방향
			Vector3 Up; //업벡터
		} lookAt[6];

		lookAt[0] = { Vector3(x + scale.x, y, z), Vector3(0, 1, 0) };
		lookAt[1] = { Vector3(x - scale.x, y, z), Vector3(0, 1, 0) };
		//왼손좌표계.
		lookAt[2] = { Vector3(x, y + scale.y, z), Vector3(0, 0, -1) };
		//앞으로 간다. 왼손으로 하면 된다. 아무축이나 z 잡고 
		lookAt[3] = { Vector3(x, y - scale.y, z), Vector3(0, 0, +1) };
		lookAt[4] = { Vector3(x, y, z + scale.z), Vector3(0, 1, 0) };
		//앞으로 간다. 왼손으로 하면 된다. 아무축이나 z 잡고 그다음에 순서가 x축 잡으면 돼. <openGl은 축의 순서가 반대인가보지 그것도 왼손좌표계써> 손의 순서가 아니라 축의 순서
		lookAt[5] = { Vector3(x, y, z - scale.z), Vector3(0, 1, 0) };

		for (UINT i = 0; i < 6; i++)
			D3DXMatrixLookAtLH(&desc.Views[i], &this->position, &lookAt[i].LookAt, &lookAt[i].Up);
	}
	//월드 뷰 프로젝션. 이제 프로젝션. 셋팅하면 되겠지.
	projection = new Perspective(1, 1, zNear, zFar, Math::PI * fov);
	projection->GetMatrix(&desc.Projection);

	//버퍼 랜더링 하고
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, dsv);
	//뷰포트
	viewport->RSSetViewport();
}

void EnvCubeMap::Render()
{
	sSrv->SetResource(srv);
}
