#ifndef TESTS_TEST_TRACERY_H_
#define TESTS_TEST_TRACERY_H_

//
// Unit tests for cpptracery
//

#include <cassert>
#include <string>
#include <vector>

#include "tracery.h"

static const char* test_grammar =
    "{"
    "    \"deepHash\": [\"\\\\#00FF00\", \"\\\\#FF00FF\"],"
    "    \"deeperHash\": [\"#deepHash#\"],"
    "    \"animal\": [\"bear\", \"cat\", \"dog\", \"fox\", \"giraffe\", \"hippopotamus\"],"
    "    \"mood\": [\"quiet\", \"morose\", \"gleeful\", \"happy\", \"bemused\", \"clever\", \"jovial\", \"vexatious\", "
    "               \"curious\", \"anxious\", \"obtuse\", \"serene\", \"demure\"],"
    "    \"nonrecursiveStory\": [\"The #pet# went to the beach.\"],"
    "    \"recursiveStory\": [\"The #pet# opened a book about[pet:#mood# #animal#] #pet.a#. #story#[pet:POP] The #pet# "
    "closed the book.\"],"
    "    \"svgColor\": [\"rgb(120,180,120)\", \"rgb(240,140,40)\", \"rgb(150,45,55)\", \"rgb(150,145,125)\", "
    "                   \"rgb(220,215,195)\", \"rgb(120,250,190)\"],"
    "    \"svgStyle\": [\"style=\\\"fill:#svgColor#;stroke-width:3;stroke:#svgColor#\\\"\"],"
    "    \"name\": [\"Cheri\", \"Fox\", \"Morgana\", \"Jedoo\", \"Brick\", \"Shadow\", \"Krox\", \"Urga\", \"Zelph\"],"
    "    \"story\": [\"#hero.capitalize# was a great #occupation#, and this song tells of #heroTheir# adventure. "
    "#hero.capitalize# #didStuff#, then #heroThey# #didStuff#, then #heroThey# went home to read a book.\"],"
    "    \"monster\": [\"dragon\", \"ogre\", \"witch\", \"wizard\", \"goblin\", \"golem\", \"giant\", \"sphinx\", "
    "                  \"warlord\"],"
    "    \"setPronouns\": ["
    "                    \"[heroThey:they][heroThem:them][heroTheir:their][heroTheirs:theirs]\","
    "                    \"[heroThey:she][heroThem:her][heroTheir:her][heroTheirs:hers]\","
    "                    \"[heroThey:he][heroThem:him][heroTheir:his][heroTheirs:his]\"],"
    "    \"setOccupation\": ["
    "                      \"[occupation:baker][didStuff:baked bread,decorated cupcakes,folded dough,made "
    "croissants,iced a cake]\","
    "                      \"[occupation:warrior][didStuff:fought #monster.a#,saved a village from #monster.a#,battled "
    "#monster.a#,defeated #monster.a#]\"],"
    "    \"origin\": [\"#[#setPronouns#][#setOccupation#][hero:#name#]story#\"]"
    "}";

class TestPytracery
{
   public:
    void assertTrue(bool condition, const std::string& msg)
    {
        assert(condition);
    }

    void assertEqual(const std::string& a, const std::string& b)
    {
        assert(a == b);
    }

    void assertEqual(const std::vector<std::string>& a, const std::vector<std::string>& b)
    {
        assert(a.size() == b.size());
        for (size_t i = 0; i < a.size(); ++i)
        {
            assert(a[i] == b[i]);
        }
    }

    void assertIn(const std::string& a, const std::string& b)
    {
        assert(b.find(a) != std::string::npos);
    }

    void assertIn(const std::string& a, const std::vector<std::string>& b)
    {
        bool found = false;
        for (size_t i = 0; i < b.size(); ++i)
        {
            if (a == b[i])
            {
                found = true;
                break;
            }
        }
        assert(found);
    }

    // Common setUp and helpers

    tracery::Grammar* grammar;

    TestPytracery()
    {
        grammar = new tracery::Grammar(test_grammar);
        grammar->add_modifiers(modifiers::base_english());
    }

    void assert_starts_with(const std::string& a, const std::string& b, std::string msg = "")
    {
        if (msg.empty())
        {
            msg = a + " does not start with " + b;
        }
        assertTrue(a.substr(0, b.length()) == b, msg);
    }

    void assert_ends_with(const std::string& a, const std::string& b, std::string msg = "")
    {
        if (msg.empty())
        {
            msg = a + " does not end with " + b;
        }

        assertTrue(a.substr(a.length() - b.length(), b.length()) == b, msg);
    }
};

extern void test_modifiers();

extern void test_tracery();

#endif  // TESTS_TEST_TRACERY_H_
