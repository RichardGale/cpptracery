//
// Unit tests for cpptracery
//

#include <string>
#include <vector>

#include "tracery.h"

#include "test_tracery.h"

class TestBasics : public TestPytracery
{
   public:
    void test_plaintext_short()
    {
        // Arrange
        std::string src = "a";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "a");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_plaintext_long()
    {
        // Arrange
        std::string src =
            "Emma Woodhouse, handsome, clever, and rich, with a "
            "comfortable home and happy disposition, seemed to unite some "
            "of the best blessings of existence; and had lived nearly "
            "twenty-one years in the world with very little to distress or "
            "vex her.";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret,
                    "Emma Woodhouse, handsome, clever, and rich, with a "
                    "comfortable home and happy disposition, seemed to unite some "
                    "of the best blessings of existence; and had lived nearly "
                    "twenty-one years in the world with very little to distress or "
                    "vex her.");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }
};

class TestEscapeChars : public TestPytracery
{
   public:
    void test_escape_character()
    {
        // Arrange
        std::string src = "\\#escape hash\\# and escape slash\\\\";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "#escape hash# and escape slash\\");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_deep()
    {
        // Arrange
        std::string src = "#deepHash# [myColor:#deeperHash#] #myColor#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        std::vector<std::string> correct;
        correct.push_back("#00FF00  #00FF00");
        correct.push_back("#00FF00  #FF00FF");
        correct.push_back("#FF00FF  #00FF00");
        correct.push_back("#FF00FF  #FF00FF");
        assertIn(ret, correct);
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_quotes()
    {
        // Arrange
        std::string src = "\"test\" and \'test\'";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "\"test\" and 'test'");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_brackets()
    {
        // Arrange
        std::string src = "\\[\\]";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "[]");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_hash()
    {
        // Arrange
        std::string src = "\\#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "#");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_unescape_char_slash()
    {
        // Arrange
        std::string src = "\\\\";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "\\");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_melange1()
    {
        // Arrange
        std::string src = "An action can have inner tags: \\[key:\\#rule\\#\\]";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "An action can have inner tags: [key:#rule#]");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_escape_melange2()
    {
        // Arrange
        std::string src =
            "A tag can have inner actions: "
            "\"\\#\\[myName:\\#name\\#\\]story\\[myName:POP\\]\\#\"";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret,
                    "A tag can have inner actions: "
                    "\"#[myName:#name#]story[myName:POP]#\"");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }
};

class TestWebSpecifics : public TestPytracery
{
   public:
    void test_emoji()
    {
        // Arrange
        std::string src = "💻🐋🌙🏄🍻";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "\U0001f4bb\U0001f40b\U0001f319\U0001f3c4\U0001f37b");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_unicode()
    {
        // Arrange
        std::string src = "&\\#x2665; &\\#x2614; &\\#9749; &\\#x2665;";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "&#x2665; &#x2614; &#9749; &#x2665;");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_svg()
    {
        // Arrange
        std::string src =
            "<svg width=\"100\" height=\"70\">"
            "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\" #svgStyle#/> "
            "<rect x=\"20\" y=\"10\" width=\"40\" height=\"30\" #svgStyle#/>"
            "</svg>";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assert_starts_with(ret,
                           "<svg width=\"100\" height=\"70\">"
                           "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\" style=\"fill:rgb(");
        assertIn("/> <rect x=\"20\" y=\"10\" width=\"40\" height=\"30\" ", ret);
        //        self.assertIn('/> <rect x="20" y="10" width="40" height="30" ', ret)
        assert_ends_with(ret, "\"/></svg>");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }
};

class TestPush : public TestPytracery
{
   public:
    void test_push_basic()
    {
        // Arrange
        std::string src = "[pet:#animal#]You have a #pet#. Your #pet# is #mood#.";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assert_starts_with(ret, "You have a ");
        assertIn(". Your ", ret);
        assert_ends_with(ret, ".");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_push_pop()
    {
        // Arrange
        std::string src =
            "[pet:#animal#]You have a #pet#. [pet:#animal#]Pet:#pet# "
            "[pet:POP]Pet:#pet#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assert_starts_with(ret, "You have a ");
        assertIn(". Pet:", ret);
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_tag_action()
    {
        // Arrange
        std::string src = "#[pet:#animal#]nonrecursiveStory# post:#pet#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assert_starts_with(ret, "The ");
        assertIn(" went to the beach. post:", ret);
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_test_complex_grammar()
    {
        // Arrange
        std::string src = "#origin#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertIn(", and this song tells of ", ret);
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_missing_modifier()
    {
        // Arrange
        std::string src = "#animal.foo#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assert_ends_with(ret, "((.foo))");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_modifier_with_params()
    {
        // Arrange
        std::string src = "[pet:#animal#]#nonrecursiveStory# -> #nonrecursiveStory.replace(beach,mall)#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertIn(" went to the beach. -> ", ret);
        assert_ends_with(ret, " went to the mall.");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_recursive_push()
    {
        // Arrange
        std::string src = "[pet:#animal#]#recursiveStory#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertIn(" opened a book about a", ret);
        assert_ends_with(ret, " closed the book.");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }
};

class TestStrings : public TestPytracery
{
   public:
    void test_string_input()
    {
        // The grammar should work properly if the input is a string rather
        // than a list
        // This errored in Py 3.5, taken straight from the README
        std::string rules =
            "{"
            "\"origin\": \"#hello.capitalize#, #location#!\","
            "\"hello\": [\"hello\"],"
            "\"location\": [\"world\"]"
            "}";

        grammar = new tracery::Grammar(rules);
        grammar->add_modifiers(modifiers::base_english());
        assertEqual("Hello, world!", grammar->flatten("#origin#"));
    }

    void test_upper_and_lowercase()
    {
        std::string rules =
            "{"
            "\"origin\": \"#hello.lowercase#, #location.uppercase#!\","
            "\"hello\": [\"Hello\"],"
            "\"location\": [\"world\"]"
            "}";

        grammar = new tracery::Grammar(rules);
        grammar->add_modifiers(modifiers::base_english());
        assertEqual("hello, WORLD!", grammar->flatten("#origin#"));
    }
};

class TestErrors : public TestPytracery
{
   public:
    void test_plaintext_short()
    {
        // Arrange
        std::string src = "a";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "a");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_unmatched_hash()
    {
        // Arrange
        std::string src = "#unmatched";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "unmatched");

        std::vector<std::string> correct;
        correct.push_back("unclosed tag");
        assertEqual(grammar->m_errors, correct);
    }

    void test_missing_symbol()
    {
        // Arrange
        std::string src = "#unicorns#";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "((unicorns))");
        assertEqual(grammar->m_errors, std::vector<std::string>());
    }

    void test_missing_right_bracket()
    {
        // Arrange
        std::string src = "[pet:unicorn";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "pet:unicorn");
        std::vector<std::string> correct;
        correct.push_back("too many [");
        assertEqual(grammar->m_errors, correct);
    }

    void test_missing_left_bracket()
    {
        // Arrange
        std::string src = "pet:unicorn]";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "pet:unicorn]");
        std::vector<std::string> correct;
        correct.push_back("too many ]");
        assertEqual(grammar->m_errors, correct);
    }

    void test_just_a_lot_of_brackets()
    {
        // Arrange
        std::string src = "[][]][][][[[]]][[]]]]";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "][][][][]]]");
        std::vector<std::string> errors;
        errors.push_back("1: empty action");
        errors.push_back("1: empty action");
        errors.push_back("3: empty action");
        errors.push_back("17: empty action");
        errors.push_back("too many ]");
        assertEqual(grammar->m_errors, errors);
    }

    void test_bracket_tag_melange()
    {
        // Arrange
        std::string src = "[][#]][][##][[[##]]][#[]]]]";

        // Act
        std::string ret = grammar->flatten(src);

        // Assert
        assertEqual(ret, "][][((None))][][[]]]]");
        std::vector<std::string> errors;
        errors.push_back("unclosed tag");
        errors.push_back("1: empty tag");
        errors.push_back("1: empty action");
        errors.push_back("10: empty tag");
        errors.push_back("unclosed tag");
        errors.push_back("too many ]");
        assertEqual(grammar->m_errors, errors);
    }
};

void test_tracery()
{
    {
        TestBasics test_basics;
        test_basics.test_plaintext_short();
    }
    {
        TestBasics test_basics;
        test_basics.test_plaintext_long();
    }

    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_character();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_deep();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_quotes();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_brackets();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_hash();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_unescape_char_slash();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_melange1();
    }
    {
        TestEscapeChars testEscapeChars;
        testEscapeChars.test_escape_melange2();
    }

    {
        TestWebSpecifics testWebSpecifics;
        testWebSpecifics.test_emoji();
    }
    {
        TestWebSpecifics testWebSpecifics;
        testWebSpecifics.test_unicode();
    }
    {
        TestWebSpecifics testWebSpecifics;
        testWebSpecifics.test_svg();
    }

    {
        TestPush testPush;
        testPush.test_push_basic();
    }
    {
        TestPush testPush;
        testPush.test_push_pop();
    }
    {
        TestPush testPush;
        testPush.test_tag_action();
    }
    {
        TestPush testPush;
        testPush.test_test_complex_grammar();
    }
    {
        TestPush testPush;
        testPush.test_missing_modifier();
    }
    {
        TestPush testPush;
        testPush.test_modifier_with_params();
    }
    {
        TestPush testPush;
        testPush.test_recursive_push();
    }

    {
        TestStrings testStrings;
        testStrings.test_string_input();
    }
    {
        TestStrings testStrings;
        testStrings.test_upper_and_lowercase();
    }

    {
        TestErrors testErrors;
        testErrors.test_plaintext_short();
    }
    {
        TestErrors testErrors;
        testErrors.test_unmatched_hash();
    }
    {
        TestErrors testErrors;
        testErrors.test_missing_symbol();
    }
    {
        TestErrors testErrors;
        testErrors.test_missing_right_bracket();
    }
    {
        TestErrors testErrors;
        testErrors.test_missing_left_bracket();
    }
    {
        TestErrors testErrors;
        testErrors.test_just_a_lot_of_brackets();
    }
    {
        TestErrors testErrors;
        testErrors.test_bracket_tag_melange();
    }
}
