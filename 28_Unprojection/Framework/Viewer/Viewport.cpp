#include "Framework.h"
#include "Viewport.h"

Viewport::Viewport(UINT width, UINT height, float x, float y, float minDepth, float maxDepth)
{
	this->width = width < 1 ? (UINT)D3D::Width() : width;
	this->height = height < 1 ? (UINT)D3D::Height() : height;

	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}

void Viewport::Set(UINT width, UINT height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.MinDepth = this->minDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	this->width = width;
	viewport.Width = (float)this->width;

	this->height = height;
	viewport.Height = (float)this->height;

	RSSetViewport();
}

void Viewport::Project(Vector3 * pOut, Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;

	Matrix wvp = W * V * P;
	D3DXVec3TransformCoord(pOut, &position, &wvp);

	pOut->x = ((pOut->x + 1.0f) * 0.5f) * width + x;
	pOut->y = ((-pOut->y + 1.0f) * 0.5f) * height + y;
	pOut->z = (pOut->z * (maxDepth - minDepth)) + minDepth;
}

void Viewport::UnProject(Vector3 * pOut, Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;
	pOut->x = ((position.x - x) / width)* 2.0f - 1.0f;
	pOut->y = ((position.y - y) / width)* 2.0f - 1.0f * -1.0f; //������ ���ٴ°� ���߿� ����� ��������Ѵ�.
	pOut->z = ((position.z - minDepth) / (maxDepth - minDepth));

	Matrix wvp = W * V*P;
	D3DXMatrixInverse(&wvp, NULL, &wvp);

	D3DXVec3TransformCoord(pOut, pOut, &wvp);
	//���� ������ ���� ī�޶� �������� ���� ��ϱ� ���� ������ ����.
}