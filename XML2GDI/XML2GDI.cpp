#include "XML2GDI.h" 
#include "tinyxml2/include/tinyxml2.h"

bool ListAllElements(std::vector<XmlWindow>& xmlWindows, tinyxml2::XMLElement* element, XmlWindow* pParentWindow = NULL)
{
    //Set Attributes.
    auto listAttributeAndSet = [](XmlWindow& xmlWindow, tinyxml2::XMLElement& element) -> void
        {
            const tinyxml2::XMLAttribute* attribute = element.FirstAttribute();

            while (attribute != NULL) {
                xmlWindow.SetAttributeValue(attribute->Name(), attribute->Value());
                attribute = attribute->Next();
            }
        };

    //Make window,
    XmlWindow window = MakeXmlWindow(element->Name(), pParentWindow);
    if (window.GetHandle() == NULL)
        return false;

    //list and set attributes to window,
    listAttributeAndSet(window, *element);

    //If the window is a child window, make it visible,
    if (pParentWindow != NULL)
    {
        SetWindowLong(window.GetHandle(), GWL_STYLE, GetWindowLong(window.GetHandle(), GWL_STYLE) | WS_CHILD);
        SetParent(window.GetHandle(), pParentWindow->GetHandle());
    }

    //Make child window,
    tinyxml2::XMLElement* subElement = element->FirstChildElement();

    while (subElement != NULL)
    {
        if (!ListAllElements(xmlWindows, subElement, &window)) 
            return false;
        subElement = subElement->NextSiblingElement();
    }

    //Add it to the set of windows.
    xmlWindows.push_back(static_cast<XmlWindow&&>(window));
    
    return true;
}

std::vector<XmlWindow> Xml2Gdi::FromXml(std::string xmlContent)
{
    //init return value
    std::vector<XmlWindow> result = {};

    //read XML Documents.
    tinyxml2::XMLDocument document;

    tinyxml2::XMLError err = document.Parse(xmlContent.c_str());

    if (err != tinyxml2::XMLError::XML_SUCCESS)
        return {};

    //Start Create window.
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
