#ifndef MODIFIERS_H_
#define MODIFIERS_H_

#include <map>
#include <string>
#include <vector>

namespace modifiers
{
    std::vector<std::string> split(std::string str,
                                   std::string sep);

    std::string replace(std::string text,
                        const std::vector<std::string>& params = std::vector<std::string>());

    std::string capitalizeAll(std::string text,
                              const std::vector<std::string>& params = std::vector<std::string>());

    std::string capitalize_(std::string text,
                            const std::vector<std::string>& params = std::vector<std::string>());

    std::string a(std::string text,
                  const std::vector<std::string>& params = std::vector<std::string>());

    std::string firstS(std::string text,
                       const std::vector<std::string>& params = std::vector<std::string>());

    std::string s(std::string text,
                  const std::vector<std::string>& params = std::vector<std::string>());

    std::string ed(std::string text,
                   const std::vector<std::string>& params = std::vector<std::string>());

    std::string uppercase(std::string text,
                          const std::vector<std::string>& params = std::vector<std::string>());

    std::string lowercase(std::string text,
                          const std::vector<std::string>& params = std::vector<std::string>());
    

    typedef std::string (*modifier_func)(std::string text, const std::vector<std::string>& params);

    std::map<std::string, modifier_func> base_english();
}

#endif
