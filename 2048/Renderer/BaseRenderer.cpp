#include "Common.h"
#include "BaseRenderer.h"
#include "GdiRenderer.h"
#include "D2DRenderer.h"
#include "D3D9Renderer.h"
#include "OpenGL11Renderer.h"

BaseRenderer::BaseRenderer(HWND hWnd)
: m_hWnd(hWnd)
{
}

BaseRenderer *CreateRenderer(HWND hWnd, RendererType type)
{
	BaseRenderer *pRenderer = nullptr;
	switch (type)
	{
	case kRendererGdi:
		pRenderer = new GdiRenderer(hWnd);
		break;
	case kRendererDirect2D:
		pRenderer = new D2DRenderer(hWnd);
		break;
	case kRendererDirect3D9:
		pRenderer = new D3D9Renderer(hWnd);
		break;
	case kRendererOpenGL11:
		pRenderer = new OpenGL11Renderer(hWnd);
		break;
	default:
		break;
	}

	return pRenderer;
}