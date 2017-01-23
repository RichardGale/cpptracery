//
// Unit tests for cpptracery
//

#include <string>
#include <vector>

#include "modifiers.h"

#include "test_tracery.h"

class TestModifiers : public TestPytracery
{
public:
    void test_replace()
    {
        // Arrange
        std::string text = "a big ship";

        // Act
        std::vector<std::string> mod_params;
        mod_params.push_back("ship");
        mod_params.push_back("shop");
        std::string output = modifiers::replace(text, mod_params);

        // Assert
        assertEqual(output, "a big shop");
    }

    void test_capitalize_all()
    {
        // Arrange
        std::string text = "abc def";

        // Act
        std::string output = modifiers::capitalizeAll(text);

        // Assert
        assertEqual(output, "Abc Def");
    }

    void test_capitalize_()
    {
        // Arrange
        std::string text = "abc def";

        // Act
        std::string output = modifiers::capitalize_(text);

        // Assert
        assertEqual(output, "Abc def");
    }

    void test_a_consonant()
    {
        // Arrange
        std::string text = "house";

        // Act
        std::string output = modifiers::a(text);

        // Assert
        assertEqual(output, "a house");
    }

    void test_a_u_something_i()
    {
        // Arrange
        std::string text = "unicorn";

        // Act
        std::string output = modifiers::a(text);

        // Assert
        assertEqual(output, "a unicorn");
    }

    void test_a_u_not_i()
    {
        // Arrange
        std::string text = "underdog";

        // Act
        std::string output = modifiers::a(text);

        // Assert
        assertEqual(output, "an underdog");
    }

    void test_a_vowel()
    {
        // Arrange
        std::string text = "elephant";

        // Act
        std::string output = modifiers::a(text);

        // Assert
        assertEqual(output, "an elephant");
    }

    void test_first_s()
    {
        // Arrange
        std::string text = "elephant in a phonebox";

        // Act
        std::string output = modifiers::firstS(text);

        // Assert
        assertEqual(output, "elephants in a phonebox");
    }

    void test_s_ends_in_x()
    {
        // Arrange
        std::string text = "box";

        // Act
        std::string output = modifiers::s(text);

        // Assert
        assertEqual(output, "boxes");
    }

    void test_s_ends_in_non_s()
    {
        // Arrange
        std::string text = "goat";

        // Act
        std::string output = modifiers::s(text);

        // Assert
        assertEqual(output, "goats");
    }

    void test_s_ends_in_vowel_y()
    {
        // Arrange
        std::string text = "monkey";

        // Act
        std::string output = modifiers::s(text);

        // Assert
        assertEqual(output, "monkeys");
    }

    void test_s_ends_in_y_but_not_vowel_y()
    {
        // Arrange
        std::string text = "telly";

        // Act
        std::string output = modifiers::s(text);

        // Assert
        assertEqual(output, "tellies");
    }

    void test_ed_ends_in_e()
    {
        // Arrange
        std::string text = "glide";

        // Act
        std::string output = modifiers::ed(text);

        // Assert
        assertEqual(output, "glided");
    }

    void test_ed_ends_in_y_but_not_vowel_y()
    {
        // Arrange
        std::string text = "shimmy";

        // Act
        std::string output = modifiers::ed(text);

        // Assert
        assertEqual(output, "shimmied");
    }

    void test_ed_ends_in_non_e_and_non_y()
    {
        // Arrange
        std::string text = "jump";

        // Act
        std::string output = modifiers::ed(text);

        // Assert
        assertEqual(output, "jumped");
    }

    void test_uppercase()
    {
        // Arrange
        std::string text = "jump up";

        // Act
        std::string output = modifiers::uppercase(text);

        // Assert
        assertEqual(output, "JUMP UP");
    }

    void test_lowercase()
    {
        // Arrange
        std::string text = "GET DOWN";

        // Act
        std::string output = modifiers::lowercase(text);

        // Assert
        assertEqual(output, "get down");
    }
};

void test_modifiers()
{
    {
        TestModifiers testModifiers;
        testModifiers.test_replace();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_capitalize_all();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_capitalize_();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_a_consonant();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_a_u_something_i();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_a_u_not_i();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_a_vowel();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_first_s();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_s_ends_in_x();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_s_ends_in_non_s();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_s_ends_in_vowel_y();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_s_ends_in_y_but_not_vowel_y();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_ed_ends_in_e();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_ed_ends_in_y_but_not_vowel_y();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_ed_ends_in_non_e_and_non_y();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_uppercase();
    }
    {
        TestModifiers testModifiers;
        testModifiers.test_lowercase();
    }
}
