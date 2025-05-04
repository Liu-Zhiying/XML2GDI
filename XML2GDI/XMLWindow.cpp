#include "XMLWindow.h"
#include "Utils.h"
#include <algorithm>

std::string GetTextAttribute(HWND hWnd)
{
	std::string result;
#if defined(UNICODE) || defined(_UNICODE)
	std::wstring text;
	text.resize(1024);
	text.resize(GetWindowTextW(hWnd, &text[0], 1024));
	result = WStr2Str(text);
#else
	result.resize(1024);
	result.resize(GetWindowTextA(hWnd, &result[0], 1024));
#endif // defined(UNICODE) || defined(_UNICODE)
	return result;
}

void SetTextAttribute(HWND hWnd, const std::string& value)
{
#if defined(UNICODE) || defined(_UNICODE)
	std::wstring text = Str2WStr(value);
	SetWindowTextW(hWnd, text.c_str());
#else
	SetWindowTextA(hWnd, value.c_str());
#endif // defined(UNICODE) || defined(_UNICODE)
	
}

std::string GetRectAttribute(HWND hWnd)
{
	RECT rect = {};
	std::string result;
	GetWindowRect(hWnd, &rect);

	result += std::to_string(rect.left);
	result += ',';
	result += std::to_string(rect.top);
	result += ',';
	result += std::to_string(rect.right);
	result += ',';
	result += std::to_string(rect.bottom);

	return result;
}

void SetRectAttribute(HWND hWnd, const std::string& value)
{
	RECT rect = {};
	sscanf_s(value.c_str(), "%d,%d,%d,%d", &rect.left, &rect.top, &rect.right, &rect.bottom);
	SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);
}

struct WindowStyleAndValue
{
	const char* styleName;
	DWORD value;
} styleInfos[] = {
	{ "WS_OVERLAPPEDWINDOW", 0x000CF0000L },
	{ "WS_POPUPWINDOW", 0x80880000L },
	{ "WS_CAPTION", 0x00C00000L },
	{ "WS_OVERLAPPED", 0x00000000L },
	{ "WS_POPUP ", 0x80000000L },
	{ "WS_CHILD", 0x40000000L },
	{ "WS_MINIMIZE", 0x20000000L },
	{ "WS_VISIBLE", 0x10000000L },
	{ "WS_DISABLED", 0x08000000L },
	{ "WS_CLIPSIBLINGS", 0x04000000L },
	{ "WS_CLIPCHILDREN", 0x02000000L },
	{ "WS_MAXIMIZE", 0x01000000L },
	{ "WS_BORDER", 0x00800000L },
	{ "WS_DLGFRAME", 0x00400000L },
	{ "WS_VSCROLL", 0x00200000L },
	{ "WS_HSCROLL", 0x00100000L },
	{ "WS_SYSMENU" ,0x00080000L },
	{ "WS_THICKFRAME", 0x00040000L },
	{ "WS_GROUP", 0x00020000L },
	{ "WS_TABSTOP", 0x00010000L },
	{ "WS_MINIMIZEBOX", 0x00020000L },
	{ "WS_MAXIMIZEBOX", 0x00010000L }
};
WindowStyleAndValue exStyleInfos[] = {
	{ "WS_EX_OVERLAPPEDWINDOW", 0x00000300L },
	{ "WS_EX_PALETTEWINDOW", 0x00000188L },
	{ "WS_EX_DLGMODALFRAM",  0x00000001L },
	{ "WS_EX_NOPARENTNOTIFY", 0x80880000 },
	{ "WS_EX_TOPMOST", 0x00000008L },
	{ "WS_EX_ACCEPTFILES", 0x00000010L },
	{ "WS_EX_TRANSPARENT", 0x00000020L },
	{ "WS_EX_MDICHILD",  0x00000040L },
	{ "WS_EX_TOOLWINDOW",  0x00000080L },
	{ "WS_EX_WINDOWEDGE",  0x00000100L },
	{ "WS_EX_CLIENTEDGE", 0x00000200L },
	{ "WS_CLIPSIBLINGS", 0x04000000L },
	{ "WS_CLIPCHILDREN", 0x02000000L },
	{ "WS_EX_CONTEXTHELP",  0x00000400L },
	{ "WS_EX_RIGHT", 0x00001000L },
	{ "WS_EX_LEFT",  0x00000000L },
	{ "WS_EX_RTLREADING", 0x00002000L },
	{ "WS_EX_LTRREADING", 0x00000000L },
	{ "WS_EX_LEFTSCROLLBAR", 0x00004000L},
	{ "WS_EX_RIGHTSCROLLBAR", 0x00000000L },
	{ "WS_EX_CONTROLPARENT", 0x00010000L },
	{ "WS_EX_STATICEDGE", 0x00020000L },
	{ "WS_EX_APPWINDOW", 0x00040000L },
	{ "WS_EX_LAYERED", 0x00080000 },
	{ "WS_EX_NOINHERITLAYOUT", 0x00100000L },
	{ "WS_EX_NOREDIRECTIONBITMAP", 0x00200000L },
	{ "WS_EX_LAYOUTRTL", 0x00400000L },
	{ "WS_EX_COMPOSITED", 0x02000000L },
	{ "WS_EX_NOACTIVATE", 0x08000000L }
};

struct WindowStyleRenam
{
	const char* keyName;
	const char* replaceName;
} styleRenames[] = {
	{ "WS_TILED", "WS_OVERLAPPED"},
	{ "WS_ICONIC", "WS_MINIMIZE"},
	{ "WS_SIZEBOX", " WS_THICKFRAME" },
	{ "WS_TILEDWINDOW", "WS_OVERLAPPEDWINDOW"},
	{ "WS_CHILDWINDOW", "WS_CHILD" }
};

std::string GetStyleAttribute(HWND hWnd)
{
	std::string styleString = {};
	DWORD styleCode = GetWindowLong(hWnd, GWL_STYLE);

	for (size_t idx = 0; idx < ARRAY_SIZE(styleInfos) && styleCode; ++idx)
	{
		
		DWORD styleTestCode = styleCode & styleInfos[idx].value;
		if (styleTestCode == styleInfos[idx].value)
		{
			//去掉测试出来的标志位
			styleCode &= ~styleInfos[idx].value;

			//添加对应的样式字符串
			styleString += styleInfos[idx].styleName;
			styleString += " | ";
		}
	}
	
	//把结尾的" | "删除
	if (styleString.length() >= 3)
	{
		for (int cnt = 0; cnt < 3; ++cnt)
			styleString.pop_back();
	}

	return styleString;
}

void SetStyleAttribute(HWND hWnd, const std::string& value)
{
	DWORD styleCode = 0;

	std::vector<std::string> words = SplitStringByChar(value, '|');

	for (auto& word : words)
	{
		std::string word_trimed = TrimStr(word);

		WindowStyleRenam* pRename = std::find_if(styleRenames, 
												 styleRenames + ARRAY_SIZE(styleRenames), 
												 [&word_trimed](WindowStyleRenam& rename) -> bool {
													 return word_trimed == rename.keyName; });

		if (pRename != styleRenames + ARRAY_SIZE(styleRenames))
			word_trimed = pRename->replaceName;

		WindowStyleAndValue* pInfo = std::find_if(styleInfos,
			styleInfos + ARRAY_SIZE(styleInfos),
			[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

		if (pInfo != styleInfos + ARRAY_SIZE(styleInfos))
		{
			styleCode |= pInfo->value;
		}
	}

	SetWindowLong(hWnd, GWL_STYLE, styleCode);
}

std::string GetExStyleAttribute(HWND hWnd)
{
	std::string styleString = {};
	DWORD styleCode = GetWindowLong(hWnd, GWL_EXSTYLE);

	for (size_t idx = 0; idx < ARRAY_SIZE(exStyleInfos) && styleCode; ++idx)
	{
		DWORD styleTestCode = styleCode & exStyleInfos[idx].value;
		if (styleTestCode == exStyleInfos[idx].value)
		{
			//去掉测试出来的标志位
			styleCode &= ~exStyleInfos[idx].value;

			//添加对应的样式字符串
			styleString += exStyleInfos[idx].styleName;
			styleString += " | ";
		}
	}

	//把结尾的" | "删除
	for (int cnt = 0; cnt < 3; ++cnt)
		styleString.pop_back();

	return styleString;
}

void SetExStyleAttribute(HWND hWnd, const std::string& value)
{
	DWORD styleCode = 0;

	std::vector<std::string> words = SplitStringByChar(value, '|');

	for (auto& word : words)
	{
		std::string word_trimed = TrimStr(word);

		WindowStyleAndValue* pInfo = std::find_if(exStyleInfos,
			exStyleInfos + ARRAY_SIZE(exStyleInfos),
			[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

		if (pInfo != exStyleInfos + ARRAY_SIZE(exStyleInfos))
			styleCode |= pInfo->value;
	}

	SetWindowLong(hWnd, GWL_EXSTYLE, styleCode);
}

//与GUI系统内置参数对应的属性
struct InternalAttributeInfo
{
	const char* key;
	std::string(*getter)(HWND hWnd);
	void (*setter)(HWND hWnd, const std::string& value);
	std::string(*checker)(const std::string& value);
} internalAttributes[] = {
	{ "rect", GetRectAttribute, SetRectAttribute },
	{ "style", GetStyleAttribute, SetStyleAttribute },
	{ "exstyle", GetExStyleAttribute, SetExStyleAttribute },
	{ "text", GetTextAttribute, SetTextAttribute }
};

std::string XmlWindow::GetTag()
{
	if (hWnd == NULL)
		return "";

	std::string result;

#if defined(UNICODE) || defined(_UNICODE)
	std::wstring className;

	className.resize(512);
	className.resize(GetClassNameW(hWnd, &className[0], 512));

	result = WStr2Str(className);
#else
	result.resize(512);
	result.resize(GetClassNameA(hWnd, &result[0], 512));
#endif // defined(UNICODE) || defined(_UNICODE)

	return result;
}

std::string XmlWindow::GetAttributeValue(const std::string& key)
{
	if (hWnd == NULL)
		return "";

	std::string value;

	InternalAttributeInfo* pInfo = std::find_if(internalAttributes,
		internalAttributes + ARRAY_SIZE(internalAttributes),
		[&key](const InternalAttributeInfo& info) -> bool { return info.key == key; });

	if (pInfo == internalAttributes + ARRAY_SIZE(internalAttributes))
		return SendMessage(hWnd, WM_GET_ATTRIBUTE_VALUE, (WPARAM)&key, (LPARAM)&value) ? value : "";
	else
		return pInfo->getter(hWnd);
}

bool XmlWindow::SetAttributeValue(const std::string& key, const std::string& value)
{
	if (hWnd == NULL)
		return "";

	InternalAttributeInfo* pInfo = std::find_if(internalAttributes,
		internalAttributes + ARRAY_SIZE(internalAttributes),
		[&key](const InternalAttributeInfo& info) -> bool { return info.key == key; });

	if (pInfo == internalAttributes + ARRAY_SIZE(internalAttributes))
		return SendMessage(hWnd, WM_GET_ATTRIBUTE_VALUE, (WPARAM)&key, (LPARAM)&value);
	else
		pInfo->setter(hWnd, value);
	return true;
}

void XmlWindow::ListAttributheAndValue(EnumAttributeAndValueFunc func)
{
	for (auto& info : internalAttributes)
		func(info.key, info.getter(hWnd));

	SendMessage(hWnd, WM_LIST_ATTRIBUTE_AND_VALUE, (WPARAM)func, 0);
}

bool XmlWindow::IsAttributeExist(const std::string& key)
{
	if (hWnd == NULL)
		return "";

	InternalAttributeInfo* pInfo = std::find_if(internalAttributes,
		internalAttributes + ARRAY_SIZE(internalAttributes),
		[&key](const InternalAttributeInfo& info) -> bool { return info.key == key; });

	if (pInfo == internalAttributes + ARRAY_SIZE(internalAttributes))
		return SendMessage(hWnd, WM_IS_ATTRIBUTE_EXIST, (WPARAM)&key, 0);
	else
		return true;
}

std::string XmlWindow::CheckAttributeValue(const std::string& key, const std::string& value)
{
	return std::string();
}

XmlWindow MakeXmlWindow(const std::string& tag)
{
#if defined(UNICODE) || defined(_UNICODE)
	std::wstring className = Str2WStr(tag);

	HWND hWnd = CreateWindowEx(0, className.c_str(), L"", 0,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
								NULL, NULL, NULL, NULL);
#else
	HWND hWnd = CreateWindowEx(0, tag.c_str(), "", 0,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								NULL, NULL, NULL, NULL);
#endif

	return XmlWindow(hWnd);
}
