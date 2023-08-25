#include <windows.h>

#include "imgui/AudioImGuiWindow.h"
#include "imgui/tools/MasterTool.h"
#include "imgui/tools/SoundsTool.h"
#include "audio/player/AudioSystem.h"
#include <imgui/backends/imgui_impl_win32.h>
#include <thread>

const char g_szClassName[] = "Audio Showcase";
uaudio::imgui::AudioImGuiWindow imGuiWindow;

std::thread audioSystemThread;

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	imGuiWindow.ProcessEvents(hwnd, msg, wParam, lParam);
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void audioThread()
{
	bool enabled = false;
	uaudio::player::audioSystem.GetEnabled(enabled);
	while (enabled)
	{
		uaudio::player::audioSystem.Update();
		uaudio::player::audioSystem.GetEnabled(enabled);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

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

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	imGuiWindow.SetHwnd(hwnd, wc);

	uaudio::imgui::MasterTool masterTool = uaudio::imgui::MasterTool();
	imGuiWindow.AddTool(masterTool);

	uaudio::imgui::SoundsTool soundsTool = uaudio::imgui::SoundsTool();
	imGuiWindow.AddTool(soundsTool);

	imGuiWindow.Initialize();

	uaudio::player::audioSystem.SetEnabled(true);
	audioSystemThread = std::thread(audioThread);

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		imGuiWindow.Render();
	}

	uaudio::player::audioSystem.SetEnabled(false);
	audioSystemThread.join();

	imGuiWindow.DeleteWindow();

	return Msg.wParam;
}