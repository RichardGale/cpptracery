#include <map>
#include <string>
#include <vector>
#include <cstddef>
#include <cstring>
#include <cctype>

#include "modifiers.h"

namespace modifiers
{
    std::vector<std::string> split(std::string str,
                                   std::string sep)
    {
        std::vector<std::string> ret;

        char* saveptr;
        char* tok = strtok_r(const_cast<char*>(str.c_str()), sep.c_str(), &saveptr);
        while (tok)
        {
            ret.push_back(tok);
            tok = strtok_r(NULL, sep.c_str(), &saveptr);
        }
        return ret;
    }

    std::string replace(std::string text,
                        const std::vector<std::string>& params)
    {
        size_t ptr = text.find(params[0]);
        if (ptr != std::string::npos)
        {
            text.replace(ptr, params[0].length(), params[1]);
        }
        return text;
    }

    std::string capitalizeAll(std::string text,
                              const std::vector<std::string>& params)
    {
        char last_char = 0;
        for (size_t i = 0; i < text.length(); ++i)
        {
            if (!isalpha(last_char))
            {
                text[i] = toupper(text[i]);
            }
            last_char = text[i];
        }
        return text;
    }

    std::string capitalize_(std::string text,
                            const std::vector<std::string>& params)
    {
        text[0] = toupper(text[0]);
        return text;
    }

    std::string a(std::string text,
                  const std::vector<std::string>& params)
    {
        if (text.length() > 0)
        {
            if (tolower(text[0]) == 'u')
            {
                if (text.length() > 2)
                {
                    if (tolower(text[2]) == 'i')
                    {
                        return std::string("a ") + text;
                    }
                }
            }
            if (std::string("aeiou").find(tolower(text[0])) != std::string::npos)
            {
                return std::string("an ") + text;
            }
        }
        return std::string("a ") + text;
    }

    std::string firstS(std::string text,
                       const std::vector<std::string>& params)
    {
        std::vector<std::string> text2 = split(text, " ");
        text = modifiers::s(text2[0]);
        for (size_t i = 1; i < text2.size(); ++i)
        {
            text += " " + text2[i];
        }
        return text;
    }

    std::string s(std::string text,
                  const std::vector<std::string>& params)
    {
        if (std::string("shx").find(text[text.length() - 1]) != std::string::npos)
        {
            return text + "es";
        }
        else if (text[text.length() - 1] == 'y')
        {
            if (std::string("aeiou").find(text[text.length() - 2]) == std::string::npos)
            {
                return text.substr(0, text.length() - 1) + "ies";
            }
            else
            {
                return text + "s";
            }
        }
        else
        {
            return text + "s";
        }
    }

    std::string ed(std::string text,
                   const std::vector<std::string>& params)
    {
        if (text[text.length() - 1] == 'e')
        {
            return text + "d";
        }
        else if (text[text.length() - 1] == 'y')
        {
            if (std::string("aeiou").find(text[text.length() - 2]) == std::string::npos)
            {
                return text.substr(0, text.length() - 1) + "ied";
            }
            return text;
        }
        else
        {
            return text + "ed";
        }
    }

    std::string uppercase(std::string text,
                          const std::vector<std::string>& params)
    {
        for (size_t i = 0; i < text.length(); ++i)
        {
            text[i] = toupper(text[i]);
        }
        return text;
    }

    std::string lowercase(std::string text,
                          const std::vector<std::string>& params)
    {
        for (size_t i = 0; i < text.length(); ++i)
        {
            text[i] = tolower(text[i]);
        }
        return text;
    }

    std::map<std::string, modifier_func> base_english()
    {
        std::map<std::string, modifier_func> mods;

        mods["replace"] = replace;
        mods["capitalizeAll"] = capitalizeAll;
        mods["capitalize"] = capitalize_;
        mods["a"] = a;
        mods["firstS"] = firstS;
        mods["s"] = s;
        mods["ed"] = ed;
        mods["uppercase"] = uppercase;
        mods["lowercase"] = lowercase;

        return mods;
    }
}  // namespace modifiers
