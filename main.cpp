#include <windows.h>

#include "imgui/AudioImGuiWindow.h"
#include "imgui/tools/MasterTool.h"
#include "imgui/tools/SoundsTool.h"
#include "imgui/tools/ChannelsTool.h"
#include "audio/player/AudioSystem.h"
#include <imgui/backends/imgui_impl_win32.h>
#include <thread>

const char g_szClassName[] = "Audio Showcase";

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	uaudio::imgui::window.ProcessEvents(hwnd, msg, wParam, lParam);
	switch (msg)
	{
		case WM_CLOSE:
		{
			uaudio::imgui::window.Stop();
			uaudio::player::audioSystem.Stop();
			uaudio::imgui::window.DeleteWindow();
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE:
		{
			uint32_t width = LOWORD(lParam);
			uint32_t height = HIWORD(lParam);
			uaudio::imgui::window.SetSize(ImVec2(width, height));
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void UpdateRenderWindow()
{
	while (uaudio::imgui::window.IsEnabled())
		uaudio::imgui::window.Render();
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wc{};
	HWND hwnd;
	MSG Msg;

	AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//Step 1: Registering the Window Class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCWSTR) g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		(LPCWSTR) g_szClassName,
		L"Audio Showcase",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1080, 720,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	uaudio::imgui::window.SetHwnd(hwnd, wc);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	uaudio::imgui::window.Initialize();

	// Step 3: Adding all the tools
	uaudio::imgui::MasterTool masterTool = uaudio::imgui::MasterTool();
	uaudio::imgui::window.AddTool(masterTool);

	uaudio::imgui::SoundsTool soundsTool = uaudio::imgui::SoundsTool();
	uaudio::imgui::window.AddTool(soundsTool);

	uaudio::imgui::ChannelsTool channelsTool = uaudio::imgui::ChannelsTool();
	uaudio::imgui::window.AddTool(channelsTool);

	uaudio::player::UAUDIO_PLAYER_RESULT result = uaudio::player::audioSystem.Start();
	if (UAUDIOPLAYERFAILED(result))
	{
		fclose(fConsole);
		return -1;
	}

	std::thread renderThread = std::thread(&UpdateRenderWindow);
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	renderThread.join();

	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	fclose(fConsole);
	return static_cast<int>(Msg.wParam);
}