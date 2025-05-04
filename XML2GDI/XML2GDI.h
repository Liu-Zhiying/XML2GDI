#ifndef XML2GDI_H
#define XML2GDI_H

#include "XMLWindow.h"
#include <vector>

class Xml2Gdi
{
public:
	static std::vector<XmlWindow> FromXml(std::string xmlContent);

	static std::string ToXml(HWND hWnd);
};



#endif
