#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "ShadowDemo.h"
#include "DepthDemo.h"
#include "ProjectorDemo.h"
#include "BloomDemo.h"
#include "GaussianBlurDemo.h"
#include "BlurDemo.h"
#include "MrtDemo.h"
#include "ColorToneDemo.h"
#include "RtvDemo.h"
#include "WeatherDemo.h"
#include "BillboardDemo.h"
#include "SpotLightDemo.h"
#include "PointLightDemo.h"
#include "NormalMapDemo.h"
#include "LightingDemo.h"

void Main::Initialize()
{
	Push(new ShadowDemo());
	//Push(new DepthDemo());
	//Push(new ProjectorDemo());
	//Push(new BloomDemo());
	//Push(new GaussianBlurDemo());
	//Push(new BlurDemo());
	//Push(new MrtDemo());
	//Push(new ColorToneDemo());
	//Push(new RtvDemo());
	//Push(new WeatherDemo());
	//Push(new BillboardDemo());
	//Push(new SpotLightDemo());
	//Push(new PointLightDemo());
	//Push(new NormalMapDemo());
	//Push(new LightingDemo());
}

void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = new Main();
	
	WPARAM wParam = Window::Run(main);
	SafeDelete(main);

	return wParam;
}