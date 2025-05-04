#include "XML2GDI.h" 
#include "tinyxml2/include/tinyxml2.h"

bool ListAllElements(std::vector<XmlWindow>& xmlWindows, tinyxml2::XMLElement* element, XmlWindow* pParentWindow = NULL)
{
    auto listAttributeAndSet = [](XmlWindow& xmlWindow, tinyxml2::XMLElement& element) -> void
        {
            const tinyxml2::XMLAttribute* attribute = element.FirstAttribute();

            while (attribute != NULL) {
                xmlWindow.SetAttributeValue(attribute->Name(), attribute->Value());
                attribute = attribute->Next();
            }
        };

    XmlWindow window = MakeXmlWindow(element->Name());
    if (window.GetHandle() == NULL)
        return false;

    listAttributeAndSet(window, *element);

    if (pParentWindow != NULL)
    {
        SetWindowLong(window.GetHandle(), GWL_STYLE, GetWindowLong(window.GetHandle(), GWL_STYLE) | WS_CHILD);
        SetParent(window.GetHandle(), pParentWindow->GetHandle());
    }

    tinyxml2::XMLElement* subElement = element->FirstChildElement();

    while (subElement != NULL)
    {
        if (!ListAllElements(xmlWindows, subElement, &window)) 
            return false;
        subElement = subElement->NextSiblingElement();
    }

    xmlWindows.push_back(static_cast<XmlWindow&&>(window));
    
    return true;
}

std::vector<XmlWindow> Xml2Gdi::FromXml(std::string xmlContent)
{
    std::vector<XmlWindow> result = {};

    tinyxml2::XMLDocument document;

    tinyxml2::XMLError err = document.Parse(xmlContent.c_str());

    if (err != tinyxml2::XMLError::XML_SUCCESS)
        return {};

    tinyxml2::XMLElement* element = document.RootElement();

    if (element != NULL)
    {
        if (!ListAllElements(result, element))
            return {};
        return result;
    }

    return {};
}

std::string Xml2Gdi::ToXml(HWND hWnd)
{
	return std::string();
}
