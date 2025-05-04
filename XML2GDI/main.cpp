#include <Windows.h>
#include "XML2GDI.h"

int main()
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof wcex;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName = L"aaa";
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wcex);

	std::vector<XmlWindow> windows = Xml2Gdi::FromXml("<aaa rect=\"100,100,900,900\" style=\"WS_OVERLAPPEDWINDOW | WS_VISIBLE\" text=\"XML2GDI Test\"><static rect=\"0,0,200,40\" text=\"bbbb\" style=\"WS_VISIBLE\"></static></aaa>");

	//printf("%x\n", GetWindowLong(windows[1].GetHandle(), GWL_STYLE));
	//printf("%s\n", windows[1].GetAttributeValue("style").c_str());

	MSG msg = {};
	//HWND hWbd = CreateWindowEx(0, L"aaa", L"aaaaa", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 40, 40, 800, 100, NULL, NULL, NULL, NULL);
	//CreateWindowEx(0, L"static", L"aaaaa", WS_CHILD | WS_VISIBLE, 40, 40, 400, 40, hWbd, NULL, NULL, NULL);

	//windows[1].Show();
	//windows[0].Show();
	//SetWindowLong(windows[0].GetHandle(), GWL_STYLE, GetWindowLong(windows[0].GetHandle(), GWL_STYLE) | SS_CENTER);


	while (GetMessage(&msg, NULL, 0, 0) >= 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}