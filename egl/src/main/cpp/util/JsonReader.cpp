#include "JsonReader.h"
#include "Common.h"
#include "FileUtils.h"

using namespace hiveVG;

CJsonReader::CJsonReader(const std::string& vFilePath)
{
    auto pAsset = CFileUtils::openFile(vFilePath.c_str());
    assert(pAsset);
    if (!pAsset)
        return;
    size_t Size = CFileUtils::getFileBytes(pAsset);
    char* pData = new char[Size];
    CFileUtils::readFile<char>(pAsset, pData, Size);
    CFileUtils::closeFile(pAsset);

    std::string JsonContent(pData, Size);

    Json::CharReaderBuilder ReaderBuilder;
    std::unique_ptr<Json::CharReader> Reader(ReaderBuilder.newCharReader());
    std::string Errors;
    if (!Reader->parse(JsonContent.c_str(), JsonContent.c_str() + JsonContent.size(), &m_Root, &Errors))
    {
        throw std::runtime_error("JSON parse error: " + Errors);
    }
}

CJsonReader::CJsonReader(const Json::Value &vJson) : m_Root(vJson)
{ }

bool CJsonReader::hasKey(const std::string& vKey) const
{
    return m_Root.isMember(vKey);
}

std::string CJsonReader::getString(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asString();
}

int CJsonReader::getInt(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asInt();
}

double CJsonReader::getDouble(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asDouble();
}

bool CJsonReader::getBool(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey].asBool();
}

Json::Value CJsonReader::getArray(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey];
}

Json::Value CJsonReader::getObject(const std::string& vKey) const
{
    __checkKeyExists(vKey);
    return m_Root[vKey];
}

void CJsonReader::__checkKeyExists(const std::string &vKey) const
{
    if (!hasKey(vKey)) throw std::runtime_error("Key not found: " + vKey);
}