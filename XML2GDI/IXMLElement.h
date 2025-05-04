#ifndef IXML_ELEMENT_H
#define IXML_ELEMENT_H

#include <string>

class IXmlElement
{
public:
	typedef void (*EnumAttributeAndValueFunc)(const std::string& key, const std::string& value);

	virtual std::string GetTag() = 0;

	virtual std::string GetAttributeValue(const std::string& key) = 0;

	//���attribute�������򴴽�
	virtual bool SetAttributeValue(const std::string& key, const std::string& value) = 0;

	virtual void ListAttributheAndValue(EnumAttributeAndValueFunc func) = 0;

	//�ж�ĳ��attribute�Ƿ����
	virtual bool IsAttributeExist(const std::string& key) = 0;

	//Ϊattribute��ֵ�ṩ�﷨��飬���ؿ��ַ�������ֵ�Ϸ�
	virtual std::string CheckAttributeValue(const std::string& key, const std::string& value) { return ""; }

	virtual ~IXmlElement() {}
};

typedef IXmlElement* (*XmlElementMaker)(const std::string& key);

#endif

