#include "EnumReflect.h"

std::string trimEnumString(const std::string &vStr)
{
    std::string::const_iterator it = vStr.begin();
    while (it != vStr.end() && std::isspace(*it)) { ++ it; }
    std::string::const_reverse_iterator rit = vStr.rbegin();
    while (rit.base() != it && std::isspace(*rit)) { ++ rit; }
    return std::string(it, rit.base());
}

std::string parseEnumString(const std::string &vStr, size_t &vLastVal, bool vIsFirst)
{
    size_t Pos = vStr.find('=', 0);
    if (Pos != std::string::npos) 
    {
        char* pEndptr = nullptr;
        std::string Val = vStr.substr(Pos + 1, vStr.size() - Pos - 1);
        unsigned long long Number = std::strtoull(Val.c_str(), &pEndptr, 10);
        if (*pEndptr == '\0') 
        {
            vLastVal = Number;
            return trimEnumString(vStr.substr(0, Pos));
        }
    	else 
        {
            vLastVal = 0;
            return std::string();
        }
    }
	else
    {
        if (vIsFirst)
            vLastVal = 0;
		else
            vLastVal++;
        return vStr;
    }
}

void splitEnumArgs(const char* vSzArgs, std::vector<size_t> &vioValueArray, std::vector<std::string> &vioSubStrArray)
{
    std::istringstream Iss(vSzArgs);
    size_t i = 0;
    size_t LastVal = 0;
    bool IsFirst = true;
    for (std::string SubStr; std::getline(Iss, SubStr, ','); ++i) 
    {
        std::string ParsedStr = parseEnumString(trimEnumString(SubStr), LastVal, IsFirst);
        vioSubStrArray.push_back(ParsedStr);
        IsFirst = false;
        vioValueArray.push_back(LastVal);
    }
}

bool icasecompare(const std::string& vStrA, const std::string& vStrB)
{
    if (vStrA.size() != vStrB.size())
        return false;
    for (size_t i = 0; i < vStrA.size(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(vStrA[i])) !=
            std::tolower(static_cast<unsigned char>(vStrB[i])))
        {
            return false;
        }
    }
    return true;
}

/*  Usage Example :
    DECLARE_ENUM(OsType, Windows = 0, Ubuntu, MacOS)

    OsType::OsType t = OsType::MacOS;
    LOG_INFO(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "%d members:", (int)OsType::Count());
    for (const auto &val : OsType::Values())
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, " %s", OsType::ToString((OsType::OsType)val));
    }

    auto B = OsType::FromString("macos");
    if (B == OsType::MacOS)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "you are write");
    }
    else
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "You are wrong");
    }

    OsType::OsType type = OsType::MacOS;
    LOG_INFO(hiveVG::TAG_KEYWORD::SYSTEMTEST_TAG, "The value of %s is: %d",
           OsType::ToString(type),
           OsType::FromString("MacOS"));
 */