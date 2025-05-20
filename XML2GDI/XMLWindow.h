#ifndef XML_WINDOW_H
#define XML_WINDOW_H

#include <Windows.h>
#include "IXMLElement.h"



//可XML搞作的GDI窗口对象
class XmlWindow : public IXmlElement
{
	HWND hWnd;
	XmlWindow* pParent;
	LRESULT (CALLBACK *pOldWndProc)(HWND hWnd, UINT32 msgId, WPARAM wParam, LPARAM lParam);
	const unsigned char* pWindowHookShellCode;

	XmlWindow(const XmlWindow&) = delete;
	XmlWindow& operator = (const XmlWindow&) = delete;

	friend const unsigned char* GenWindowHook(XmlWindow& _this);

	LRESULT XmlWindowProc(HWND hWnd, UINT32 msgId, WPARAM wParam, LPARAM lParam);

	void ResetObjectt();

public:
	XmlWindow() : hWnd(NULL), pParent(NULL), pOldWndProc(NULL), pWindowHookShellCode(NULL) {}
	XmlWindow(HWND _hWnd, XmlWindow* _pParent);

	virtual std::string GetTag() override;

	virtual std::string GetAttributeValue(const std::string& key) override;

	//如果attribute不存在则创建
	virtual bool SetAttributeValue(const std::string& key, const std::string& value) override;

	virtual void ListAttributheAndValue(EnumAttributeAndValueFunc func) override;

	//判断某个attribute是否存在
	virtual bool IsAttributeExist(const std::string& key) override;

	//为attribute的值提供语法检查，返回空字符串代表值合法
	virtual std::string CheckAttributeValue(const std::string& key, const std::string& value) override;

	HWND GetHandle() { return hWnd; }
	XmlWindow* GetParent() { return pParent; }
	void SetHandle(HWND _hWnd) { hWnd = _hWnd; }
	void Close() { if (hWnd != NULL) DestroyWindow(hWnd); }
	void Show() { ShowWindow(hWnd, SW_SHOW); }
	void Hide() { ShowWindow(hWnd, SW_HIDE); }

	virtual ~XmlWindow();

	XmlWindow(XmlWindow&& other) noexcept;

	XmlWindow& operator = (XmlWindow&& other) noexcept;
};

//非内置属性，XMLWindow会通过如下的消息交给消息处理函数处理
//可以在编写消息处理函数时处理这些消息扩展XMLWindow对应实际窗口的属性

//wParam = std::string* key
//return bool (is key exist)
#define WM_IS_ATTRIBUTE_EXIST (WM_USER + 0x300)
//wParam = std::string* key
//lParam = std::string* value
//return bool (value is getted.)
//write value to std::string pointed by lParam.
#define WM_GET_ATTRIBUTE_VALUE (WM_USER + 0x301)
//wParam = std::string* key
//lParam = std::string* value
//return bool (value is setted.)
#define WM_SET_ATTRIBUTE_VALUE (WM_USER + 0x302)
//wParam = EnumAttributeAndValueFunc func
//no return
//please run this function for all atrributes and values.
#define WM_LIST_ATTRIBUTE_AND_VALUE (WM_USER + 0x303)
#define WM_CHECK_ATTRIBUTE_VALUE (WM_USER + 0x304)

XmlWindow MakeXmlWindow(const std::string& tag, XmlWindow* pParent = NULL);

#endif // !XML_WINDOWD_H

