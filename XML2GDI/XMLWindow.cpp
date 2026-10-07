#include "XMLWindow.h"
#include "Utils.h"
#include <algorithm>
#include <exception>

const unsigned char x64WindowHookShellCode[] = { 0x41, 0x51, 0x41, 0x50, 0x52, 0x51, 0x48, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x41, 0x58, 0x41, 0x59, 0x48, 0x83, 0xec, 0x20, 0xff, 0xd0, 0x48, 0x83, 0xc4, 0x20, 0x59, 0xc3 };

const unsigned char x86WindowHookShellCode[] = { 0xb9, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0 };

const unsigned char* GenWindowHook(XmlWindow& _this)
{
	SYSTEM_INFO systemInfo = {};

	GetSystemInfo(&systemInfo);

	switch (systemInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
	{
		unsigned char* shellCode = NULL;

		//Allocate executable memory.
		shellCode = (unsigned char*)VirtualAlloc(NULL, sizeof(x64WindowHookShellCode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (shellCode == NULL)
			break;

		//Copy hook template.
		memcpy(shellCode, (void*)x64WindowHookShellCode, sizeof x64WindowHookShellCode);

		//Change pointer.
		void** pThis = (void**)(shellCode + 0x8);
		void** pFunc = (void**)(shellCode + 0x12);

		*pThis = &_this;
		LRESULT(XmlWindow:: * _pFunc)(HWND hWnd, UINT32 msgId, WPARAM wParam, LPARAM lParam) = &XmlWindow::XmlWindowProc;
		*pFunc = *reinterpret_cast<void**>(&_pFunc);

		return shellCode;
	}
	case PROCESSOR_ARCHITECTURE_INTEL:
	{
		//Like last case.
		unsigned char* shellCode = NULL;

		shellCode = (unsigned char*)VirtualAlloc(NULL, sizeof(x86WindowHookShellCode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (shellCode == NULL)
			break;

		memcpy(shellCode, (void*)x86WindowHookShellCode, sizeof x86WindowHookShellCode);

		void** pThis = (void**)(shellCode + 0x1);
		void** pFunc = (void**)(shellCode + 0x6);

		*pThis = &_this;
		LRESULT(XmlWindow:: * _pFunc)(HWND hWnd, UINT32 msgId, WPARAM wParam, LPARAM lParam) = &XmlWindow::XmlWindowProc;
		*pFunc = *reinterpret_cast<void**>(&_pFunc);

		return shellCode;
	}
	default:
		throw std::exception("Architecture not supported.");
		break;
	}
	return NULL;
}

//Set new this pointer in hook.
void ChangeThisPtrForWindowHook(const unsigned char* pWindowHook, XmlWindow& _this)
{
	SYSTEM_INFO systemInfo = {};

	GetSystemInfo(&systemInfo);

	switch (systemInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
	{
		void** pThis = (void**)(pWindowHook + 0x8);
		*pThis = &_this;
		break;
	}
	case PROCESSOR_ARCHITECTURE_INTEL:
	{
		void** pThis = (void**)(pWindowHook + 0x1);
		*pThis = &_this;
		break;
	}
	default:
		throw std::exception("Architecture not supported.");
		break;
	}
}

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
	{ "WS_MAXIMIZEBOX", 0x00010000L },
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

WindowStyleAndValue buttonStyleInfos[] =
{
	{ "BS_AUTOCHECKBOX", 0x0003 },
	{ "BS_3STATE", 0x0005 },
	{ "BS_AUTO3STATE", 0x0006 },
	{ "BS_GROUPBOX", 0x0007 },
	{ "BS_AUTORADIOBUTTON", 0x0009 },
	{ "BS_PUSHBOX", 0x000A },
	{ "BS_OWNERDRAW", 0x000B },
	{ "BS_DEFPUSHBUTTON", 0x0001 },
	{ "BS_CHECKBOX", 0x0002 },
	{ "BS_RADIOBUTTON", 0x0004 },
	{ "BS_USERBUTTON", 0x0008 },
	{ "BS_PUSHBUTTON", 0x0000 }
};

WindowStyleAndValue editStyleInfos[] =
{
	{ "ES_LEFT", 0x0000 },
	{ "ES_CENTER", 0x0001 },
	{ "ES_RIGHT", 0x0002 },
	{ "ES_MULTILINE", 0x0004 },
	{ "ES_UPPERCASE", 0x0008 },
	{ "ES_LOWERCASE", 0x0010 },
	{ "ES_PASSWORD", 0x0020 },
	{ "ES_AUTOVSCROLL", 0x0040 },
	{ "ES_AUTOHSCROLL", 0x0080 },
	{ "ES_NOHIDESEL", 0x0100 },
	{ "ES_OEMCONVERT", 0x0400 },
	{ "ES_READONLY", 0x0800 },
	{ "ES_WANTRETURN", 0x1000 },
	{ "ES_NUMBER", 0x2000 },
	{ "ES_SAVESEL", 0x8000 },
};

WindowStyleAndValue staticStyleInfos[] =
{
	{ "SS_RIGHT", 0x0002 },
	{ "SS_ICON", 0x0003 },
	{ "SS_GRAYRECT", 0x0005 },
	{ "SS_BLACKFRAME", 0x0007 },
	{ "SS_WHITEFRAME", 0x0009 },
	{ "SS_USERITEM", 0x000A },
	{ "SS_ETCHEDVERT", 0x0011 },
	{ "SS_ETCHEDFRAME", 0x0012 },
	{ "SS_TYPEMASK", 0x001F },
	{ "SS_BITMAP", 0x000E },
	{ "SS_ENHMETAFILE", 0x000F },
	{ "SS_SIMPLE", 0x000B },
	{ "SS_OWNERDRAW", 0x000D },
	{ "SS_CENTER", 0x0001 },
	{ "SS_BLACKRECT", 0x0004 },
	{ "SS_WHITERECT", 0x0006 },
	{ "SS_GRAYFRAME", 0x0008 },
	{ "SS_LEFTNOWORDWRAP", 0x000C },
	{ "SS_ETCHEDHORZ", 0x0010 },
	{ "SS_NOPREFIX", 0x0080 },
	{ "SS_CENTERIMAGE", 0x0200 },
	{ "SS_RIGHTJUST", 0x0400 },
	{ "SS_REALSIZEIMAGE", 0x0800 },
	{ "SS_SUNKEN", 0x1000 },
	{ "SS_ENDELLIPSIS", 0x4000 },
	{ "SS_PATHELLIPSIS", 0x8000 },
	{ "SS_WORDELLIPSIS", 0xC000 },
	{ "SS_NOTIFY", 0x0100 },
	{ "SS_LEFT", 0x0000 },
};

WindowStyleAndValue listBoxStyleInfos[] =
{
	{ "LBS_NOTIFY", 0x0001 },
	{ "LBS_SORT", 0x0002 },
	{ "LBS_NOREDRAW", 0x0004 },
	{ "LBS_MULTIPLESEL", 0x0008 },
	{ "LBS_OWNERDRAWFIXED", 0x0010 },
	{ "LBS_OWNERDRAWVARIABLE", 0x0020 },
	{ "LBS_HASSTRINGS", 0x0040 },
	{ "LBS_USETABSTOPS", 0x0080 },
	{ "LBS_NOINTEGRALHEIGHT", 0x0100 },
	{ "LBS_MULTICOLUMN", 0x0200 },
	{ "LBS_WANTKEYBOARDINPUT", 0x0400 },
	{ "LBS_EXTENDEDSEL", 0x0800 },
	{ "LBS_DISABLENOSCROLL", 0x1000 },
	{ "LBS_NODATA", 0x2000 },
	{ "LBS_NOSEL", 0x4000 },
	{ "LBS_COMBOBOX", 0x8000 },
};

WindowStyleAndValue comboBoxStyleInfos[] =
{
	{ "CBS_DROPDOWNLIST", 0x0003 },
	{ "CBS_SIMPLE", 0x0001 },
	{ "CBS_DROPDOWN", 0x0002 },
	{ "LBS_OWNERDRAWFIXED", 0x0010 },
	{ "LBS_OWNERDRAWVARIABLE", 0x0020 },
	{ "CBS_AUTOHSCROLL", 0x0040 },
	{ "CBS_OEMCONVERT", 0x0080 },
	{ "CBS_SORT", 0x0100 },
	{ "CBS_HASSTRINGS", 0x0200 },
	{ "CBS_NOINTEGRALHEIGHT", 0x0400 },
	{ "CBS_DISABLENOSCROLL", 0x0800 },
	{ "CBS_UPPERCASE", 0x2000 },
	{ "CBS_LOWERCASE", 0x4000 },
};

WindowStyleAndValue scrollBarStyleInfos[] =
{
	{ "SBS_VERT", 0x0001 },
	{ "SBS_TOPALIGN", 0x0002 },
	{ "SBS_LEFTALIGN", 0x0002 },
	{ "SBS_BOTTOMALIGN", 0x0004 },
	{ "SBS_RIGHTALIGN", 0x0004 },
	{ "SBS_SIZEBOX", 0x0008 },
	{ "SBS_SIZEGRIP", 0x0010 },
	{ "SBS_HORZ", 0x0000 },
};


struct WindowStyleRenam
{
	const char* keyName;
	const char* replaceName;
} styleRenames[] = {
	{ "WS_TILED", "WS_OVERLAPPED" },
	{ "WS_ICONIC", "WS_MINIMIZE" },
	{ "WS_SIZEBOX", " WS_THICKFRAME" },
	{ "WS_TILEDWINDOW", "WS_OVERLAPPEDWINDOW" },
	{ "WS_CHILDWINDOW", "WS_CHILD" }
};

std::string GetWindowClassName(HWND hWnd)
{
	CHAR buffer[512] = {};
	GetClassNameA(hWnd, buffer, sizeof buffer / sizeof buffer[0]);
	return buffer;
}

std::string GetStyleAttribute(HWND hWnd)
{
	std::string styleString = {};
	DWORD styleCode = GetWindowLong(hWnd, GWL_STYLE);

	std::string className = GetWindowClassName(hWnd);
	for (auto& c : className) c = tolower(c);

	if (className == "edit")
	{
		for (auto& si : editStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	if (className == "button")
	{
		for (auto& si : buttonStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	if (className == "static")
	{
		for (auto& si : staticStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	if (className == "listbox")
	{
		for (auto& si : listBoxStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	if (className == "combobox")
	{
		for (auto& si : comboBoxStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	if (className == "scrollbar")
	{
		for (auto& si : scrollBarStyleInfos)
		{
			DWORD styleTestCode = styleCode & si.value;
			if (styleTestCode == si.value)
			{
				//去掉测试出来的标志位
				styleCode &= ~si.value;

				//添加对应的样式字符串
				styleString += si.styleName;
				styleString += " | ";
			}
		}
	}

	for (auto& si : styleInfos)
	{
		DWORD styleTestCode = styleCode & si.value;
		if (styleTestCode == si.value)
		{
			//去掉测试出来的标志位
			styleCode &= ~si.value;

			//添加对应的样式字符串
			styleString += si.styleName;
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

	std::string className = GetWindowClassName(hWnd);
	for (auto& c : className) c = tolower(c);

	for (auto& word : words)
	{
		std::string word_trimed = TrimStr(word);

		WindowStyleRenam* pRename = std::find_if(styleRenames,
			styleRenames + ARRAY_SIZE(styleRenames),
			[&word_trimed](WindowStyleRenam& rename) -> bool {
				return word_trimed == rename.keyName; });

		if (pRename != styleRenames + ARRAY_SIZE(styleRenames))
			word_trimed = pRename->replaceName;

		if (className == "edit")
		{
			WindowStyleAndValue* pInfo = std::find_if(editStyleInfos,
				editStyleInfos + ARRAY_SIZE(editStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != editStyleInfos + ARRAY_SIZE(editStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

		if (className == "button")
		{
			WindowStyleAndValue* pInfo = std::find_if(buttonStyleInfos,
				buttonStyleInfos + ARRAY_SIZE(buttonStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != buttonStyleInfos + ARRAY_SIZE(buttonStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

		if (className == "static")
		{
			WindowStyleAndValue* pInfo = std::find_if(staticStyleInfos,
				staticStyleInfos + ARRAY_SIZE(staticStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != staticStyleInfos + ARRAY_SIZE(staticStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

		if (className == "listbox")
		{
			WindowStyleAndValue* pInfo = std::find_if(listBoxStyleInfos,
				listBoxStyleInfos + ARRAY_SIZE(listBoxStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != listBoxStyleInfos + ARRAY_SIZE(listBoxStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

		if (className == "combobox")
		{
			WindowStyleAndValue* pInfo = std::find_if(comboBoxStyleInfos,
				comboBoxStyleInfos + ARRAY_SIZE(comboBoxStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != comboBoxStyleInfos + ARRAY_SIZE(comboBoxStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

		if (className == "scrollbar")
		{
			WindowStyleAndValue* pInfo = std::find_if(scrollBarStyleInfos,
				scrollBarStyleInfos + ARRAY_SIZE(scrollBarStyleInfos),
				[&word_trimed](WindowStyleAndValue& info) -> bool { return info.styleName == word_trimed; });

			if (pInfo != scrollBarStyleInfos + ARRAY_SIZE(scrollBarStyleInfos))
			{
				styleCode |= pInfo->value;
			}
		}

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
	if (styleString.length() > 3)
	{
		for (int cnt = 0; cnt < 3; ++cnt)
			styleString.pop_back();
	}

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

LRESULT XmlWindow::XmlWindowProc(HWND hWnd, UINT32 msgId, WPARAM wParam, LPARAM lParam)
{
	//Reserved, just call old window process funnction. 
	return pOldWndProc(hWnd, msgId, wParam, lParam);
}

void XmlWindow::ResetObjectt()
{
	hWnd = NULL;
	pParent = NULL;
	pWindowHookShellCode = NULL;
	pOldWndProc = NULL;
}

XmlWindow::XmlWindow(HWND _hWnd, XmlWindow* _pParent) : hWnd(_hWnd), pParent(_pParent)
{
	//Generate window process function hook.
	pWindowHookShellCode = GenWindowHook(*this);

	if (pWindowHookShellCode == NULL)
		throw std::exception("Can not generate window hook.");

	//Set window processs function to the hook and get old function.
	pOldWndProc = (LRESULT(CALLBACK*)(HWND, UINT32, WPARAM, LPARAM))SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pWindowHookShellCode);

	if (GetLastError() != ERROR_SUCCESS)
		printf("Can not set window process functtion.\n");
}

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

	if (result.length() == 0)
		result = "IUnkonwn";

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

void XmlWindow::ListAttributheAndValue(EnumAttributeAndValueFunc func, void* param)
{
	for (auto& info : internalAttributes)
		func(info.key, info.getter(hWnd), param);

	if (pOldWndProc != NULL)
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

void XmlWindow::Close()
{
	if (hWnd != NULL)
	{
		DestroyWindow(hWnd);
		hWnd = NULL;
	}
	if (pWindowHookShellCode != NULL)
	{
		VirtualFree((void*)pWindowHookShellCode, 0, MEM_RELEASE);
		pWindowHookShellCode = NULL;
	}
}

XmlWindow::~XmlWindow()
{
	CloseObject();
}

void XmlWindow::CloseObject()
{
	Close();
	ResetObjectt();
}

void XmlWindow::FreeResource()
{
	if (pWindowHookShellCode != NULL)
	{
		if (pOldWndProc != NULL)
		{
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pOldWndProc);
			pOldWndProc = NULL;
		}
		VirtualFree((void*)pWindowHookShellCode, 0, MEM_RELEASE);
		pWindowHookShellCode = NULL;
	}
	ResetObjectt();
}

XmlWindow::XmlWindow(XmlWindow&& other) noexcept
{
	*this = dynamic_cast<XmlWindow&&>(other);
}

XmlWindow& XmlWindow::operator=(XmlWindow&& other) noexcept
{
	if (this == &other)
		return *this;

	CloseObject();

	hWnd = other.hWnd;
	pParent = other.pParent;
	pOldWndProc = other.pOldWndProc;
	pWindowHookShellCode = other.pWindowHookShellCode;

	//Disabble window to pause window process function calling from system.
	EnableWindow(hWnd, FALSE);

	//Change this ptr in hook.
	ChangeThisPtrForWindowHook(pWindowHookShellCode, *this);

	//Renable window.
	EnableWindow(hWnd, TRUE);

	other.ResetObjectt();

	return *this;
}

XmlWindow MakeXmlWindow(const std::string& tag, XmlWindow* pParent)
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

	return XmlWindow(hWnd, pParent);
}
