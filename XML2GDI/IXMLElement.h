#ifndef IXML_ELEMENT_H
#define IXML_ELEMENT_H

#include <string>

class IXmlElement
{
public:
	typedef void (*EnumAttributeAndValueFunc)(const std::string& key, const std::string& value);

	virtual std::string GetTag() = 0;

	virtual std::string GetAttributeValue(const std::string& key) = 0;

	//如果attribute不存在则创建
	virtual bool SetAttributeValue(const std::string& key, const std::string& value) = 0;

	virtual void ListAttributheAndValue(EnumAttributeAndValueFunc func) = 0;

	//判断某个attribute是否存在
	virtual bool IsAttributeExist(const std::string& key) = 0;

	//为attribute的值提供语法检查，返回空字符串代表值合法
	virtual std::string CheckAttributeValue(const std::string& key, const std::string& value) { return ""; }

	virtual ~IXmlElement() {}
};

typedef IXmlElement* (*XmlElementMaker)(const std::string& key);

#endif

