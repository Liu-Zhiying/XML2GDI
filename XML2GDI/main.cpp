#include <Windows.h>
#include "XML2GDI.h"	

int main()
{
	WNDCLASSEX wcex = {};

	wcex.cbSize = sizeof wcex;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName = L"aaa";
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wcex);

	std::vector<XmlWindow> windows = Xml2Gdi::FromXml("<aaa rect=\"100,100,900,900\" style=\"WS_OVERLAPPEDWINDOW | WS_VISIBLE\" text=\"XML2GDI Test\"><static rect=\"0,0,200,40\" text=\"bbbb\" style=\"WS_VISIBLE | SS_WHITEFRAME | SS_CENTER\"></static><button rect=\"300,300,900,900\" text=\"aaa\" style=\"WS_VISIBLE | BS_3STATE\"/></aaa>");

	std::string ss = Xml2Gdi::ToXml(GetDesktopWindow());

	//printf("AAA: %llu %s\n", ss.length(), ss.c_str());
	
	windows[1].GetAttributeValue("style");

	for (SIZE_T index = 0; index < windows.size(); ++index)
	{
		printf("%s\n", windows[index].GetTag().c_str());
	}

	printf("%x\n", GetWindowLong(windows[0].GetHandle(), GWL_STYLE));

	printf("%s\n", windows[0].GetAttributeValue("style").c_str());

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0) >= 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}