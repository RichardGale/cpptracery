//
// Output an example line from a JSON Tracery grammar.
//

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "tracery.h"

using namespace tracery;
using namespace modifiers;

int main(int argc, char** argv)
{
    std::string rules;

    if (argc == 2)
    {
        std::ifstream file(argv[1]);
        if (file.is_open())
        {
            std::string line;
            while (getline(file,line))
            {
                rules += line;
            }
            file.close();
        }
    }

    Grammar grammar = tracery::Grammar(rules);
    grammar.add_modifiers(base_english());
    std::cout << grammar.flatten("#origin#") << std::endl << std::endl;

    return 0;
}
