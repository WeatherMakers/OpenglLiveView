#pragma once

#include <vector>
#include <string>
#include <sstream>

/**
 * @brief Search and remove whitespace from both ends of the string
 * @param vStr:  source string
 * @return new string
 * 
 */
extern std::string trimEnumString(const std::string &vStr);

/**
 * @brief Extract the member name string from the assignment
 *                statement, e.g "xxx = 3"
 * @param vStr:      source string
 * @param vLastVal:  previous enum value, in/out
 * @param vIsFirst:  current value is the first value of enum or not
 * @return member name string, e.g xxx
 * 
 */
extern std::string parseEnumString(const std::string &vStr, size_t &vLastVal, bool vIsFirst);

/**
 * @brief Extract a collection of substrings from strings to Array
 * @param vSzArgs:         source strings
 * @param vioValueArray:   output array, store the collection of values
 * @param vioSubStrArray:  output array, store the collection of substrings
 * @return None
 * 
 */
extern void splitEnumArgs(const char* vSzArgs, std::vector<size_t> &vioValueArray, std::vector<std::string> &vioSubStrArray);

/**
 * @brief Extract the member name string from the assignment
 * @param vStrA:  source string
 * @param vStrB:  another string
 * @return is string equal or not
 *
 */
extern bool icasecompare(const std::string& vStrA, const std::string& vStrB);

#define DECLARE_ENUM(ename, ...)                                    \
    namespace ename                                                 \
    {                                                               \
        enum ename { __VA_ARGS__ };                                 \
        static std::vector<size_t> _Enum;                           \
        static std::vector<std::string> _Strings;                   \
        inline const std::vector<size_t> &Values() { return _Enum; }\
        inline size_t Count()                                       \
        {                                                           \
            if (_Strings.empty())                                   \
			{                                                       \
                splitEnumArgs(#__VA_ARGS__, _Enum, _Strings);       \
            }                                                       \
            return _Strings.size();                                 \
        }                                                           \
        inline const char* ToString(ename e)                        \
		{                                                           \
            if (_Strings.empty())                                   \
            {                                                       \
                splitEnumArgs(#__VA_ARGS__, _Enum, _Strings);       \
            }                                                       \
            for (size_t i = 0; i < Count(); ++ i)                   \
			{                                                       \
                if (_Enum.at(i) == e)                               \
                {                                                   \
                    return _Strings[i].c_str();                     \
                }                                                   \
            }                                                       \
            return "unknow-value";                                  \
        }                                                           \
        inline ename FromString(const std::string& vStr)            \
		{                                                           \
			if (_Strings.empty())                                   \
			{                                                       \
                splitEnumArgs(#__VA_ARGS__, _Enum, _Strings);       \
            }                                                       \
            for (size_t i = 0; i < Count(); ++i)                    \
			{                                                       \
                if (icasecompare(_Strings.at(i), vStr))             \
				{                                                   \
                    return (ename)_Enum.at(i);                      \
                }                                                   \
            }                                                       \
            return (ename)(_Enum.at(0));                            \
        }                                                           \
    }