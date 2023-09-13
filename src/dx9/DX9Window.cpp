#include "dx9/DX9Window.h"

#include <d3d9.h>

namespace uaudio
{
	namespace dx9
	{
		DX9Window::DX9Window()
		{
		}
		
		DX9Window::~DX9Window()
		{
			if (g_pd3dDevice)
			{
				g_pd3dDevice->Release();
				g_pd3dDevice = nullptr;
			}
			if (g_pD3D)
			{
				g_pD3D->Release();
				g_pD3D = nullptr;
			}
			delete g_d3dpp;
		}

		void DX9Window::Init(HWND hwnd)
		{
			D3DPRESENT_PARAMETERS g_d3dpp = {};

			if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
				return;

			// Create the D3DDevice
			ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
			g_d3dpp.Windowed = TRUE;
			g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
			g_d3dpp.EnableAutoDepthStencil = TRUE;
			g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
			//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
			if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
				return;

			return;
		}

		void DX9Window::SetRenderState()
		{
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		}

		void DX9Window::Clear()
		{
			float r = 0, g = 0, b = 0;
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f), (int)(1 * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		}

		bool DX9Window::BeginScene()
		{
			return g_pd3dDevice->BeginScene() >= 0;
		}

		bool DX9Window::EndScene()
		{
			return g_pd3dDevice->EndScene();
		}

		bool DX9Window::CreateDeviceD3D(HWND hWnd)
		{
			if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
				return false;

			// Create the D3DDevice
			g_d3dpp = new _D3DPRESENT_PARAMETERS_();
			ZeroMemory(g_d3dpp, sizeof(&g_d3dpp));
			g_d3dpp->Windowed = TRUE;
			g_d3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
			g_d3dpp->BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
			g_d3dpp->EnableAutoDepthStencil = TRUE;
			g_d3dpp->AutoDepthStencilFormat = D3DFMT_D16;
			g_d3dpp->PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
			//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
			if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, g_d3dpp, &g_pd3dDevice) < 0)
				return false;

			return true;
		}

		void DX9Window::CleanupDeviceD3D()
		{
			if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
			if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
		}

		void DX9Window::ResetDevice()
		{
			HRESULT hr = g_pd3dDevice->Reset(g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
			{

			}
		}
	}
}