#ifndef XML_WINDOW_H
#define XML_WINDOW_H

#include <Windows.h>
#include "IXMLElement.h"

//��XML������GDI���ڶ���
class XmlWindow : public IXmlElement
{
	HWND hWnd;

	XmlWindow(const XmlWindow&) = delete;
	XmlWindow& operator = (const XmlWindow&) = delete;

public:
	XmlWindow() : hWnd(NULL) {}
	XmlWindow(HWND _hWnd) : hWnd(_hWnd) {}

	virtual std::string GetTag() override;

	virtual std::string GetAttributeValue(const std::string& key) override;

	//���attribute�������򴴽�
	virtual bool SetAttributeValue(const std::string& key, const std::string& value) override;

	virtual void ListAttributheAndValue(EnumAttributeAndValueFunc func) override;

	//�ж�ĳ��attribute�Ƿ����
	virtual bool IsAttributeExist(const std::string& key) override;

	//Ϊattribute��ֵ�ṩ�﷨��飬���ؿ��ַ�������ֵ�Ϸ�
	virtual std::string CheckAttributeValue(const std::string& key, const std::string& value) override;

	HWND GetHandle() { return hWnd; }
	void SetHandle(HWND _hWnd) { hWnd = _hWnd; }
	void Close() { DestroyWindow(hWnd); }
	void Show() { ShowWindow(hWnd, SW_SHOW); }
	void Hide() { ShowWindow(hWnd, SW_HIDE); }

	virtual ~XmlWindow() { Close(); }

	XmlWindow(XmlWindow&& other) noexcept
	{
		*this = static_cast<XmlWindow&&>(other);
	}
	XmlWindow& operator = (XmlWindow&& other) noexcept
	{
		Close();
		hWnd = other.hWnd;
		other.hWnd = NULL;
		return *this;
	}
};

//���������ԣ�XMLWindow��ͨ�����µ���Ϣ������Ϣ����������
//�����ڱ�д��Ϣ������ʱ������Щ��Ϣ��չXMLWindow��Ӧʵ�ʴ��ڵ�����

//wParam = std::string* key
//return bool (is key exist)
#define WM_IS_ATTRIBUTE_EXIST WM_USER + 0x300
//wParam = std::string* key
//lParam = std::string* value
//return bool (value is getted.)
//write value to std::string pointed by lParam.
#define WM_GET_ATTRIBUTE_VALUE WM_USER + 0x301
//wParam = std::string* key
//lParam = std::string* value
//return bool (value is setted.)
#define WM_SET_ATTRIBUTE_VALUE WM_USER + 0x302
//wParam = EnumAttributeAndValueFunc func
//no return
//please run this function for all atrributes and values.
#define WM_LIST_ATTRIBUTE_AND_VALUE WM_USER + 0x303
#define WM_CHECK_ATTRIBUTE_VALUE WM_USER+ 0x304

XmlWindow MakeXmlWindow(const std::string& tag);

#endif // !XML_WINDOWD_H

