#pragma once

#include <windows.h>

struct IDirect3D9;
struct IDirect3DDevice9;

namespace uaudio
{
	namespace dx9
	{
		class DX9Window
		{
		public:
			DX9Window();
			~DX9Window();

			void Init(HWND hwnd);
			void SetRenderState();
			void Clear();
			bool BeginScene();
			bool EndScene();

			IDirect3D9* g_pD3D = nullptr;
			IDirect3DDevice9* g_pd3dDevice = nullptr;
		};
	}
}