#ifndef TRACERY_TRACERY_H_
#define TRACERY_TRACERY_H_

#include <map>
#include <string>
#include <vector>
#include <cstdlib>

#include "modifiers.h"

namespace tracery
{
    class Grammar;
    class NodeAction;

    class Node
    {
     public:
        Grammar* m_grammar;
        Node* m_parent;
        int m_depth;
        int m_child_index;
        std::string m_raw;
        int m_type;
        bool m_is_expanded;

        std::vector<std::string> m_errors;

        std::vector<Node*> m_children;
        std::string m_finished_text;

        std::string m_child_rule;

        std::string m_symbol;
        std::vector<NodeAction*> m_preactions;
        std::vector<NodeAction*> m_postactions;
        std::vector<std::string> m_modifiers;
        NodeAction* m_action;

        Node(Grammar* parent,
             int child_index,
             std::map<std::string, std::string> settings);

        Node(Node* parent,
             int child_index,
             std::map<std::string, std::string> settings);

        void expand_children(const std::string& child_rule,
                             bool prevent_recursion = false);

        void expand(bool prevent_recursion = false);

        void clear_escape_chars();
    };

    class NodeAction
    {
       public:
        Node* m_node;
        std::string m_target;
        int m_type;
        std::string m_rule;

        std::vector<std::string> m_rule_sections;
        std::vector<std::string> m_finished_rules;

        // has a 'raw' attribute
        NodeAction(Node* node,
                   const std::string& raw);

        NodeAction* create_undo();

        void activate();

        void to_text();
    };

    class RuleSet
    {
       public:
        std::vector<std::string> m_raw;
        Grammar* m_grammar;
        std::vector<std::string> m_default_uses;
        std::vector<std::string> m_default_rules;

        RuleSet(Grammar* grammar,
                const std::string& raw);

        RuleSet(Grammar* grammar,
                const std::vector<std::string>& raw);

        std::string select_rule();

        void clear_state();
    };

    class Symbol
    {
       public:
        Grammar* m_grammar;
        std::string m_key;
        std::vector<std::string> m_raw_rules;
        RuleSet* m_base_rules;
        std::vector<RuleSet*> m_stack;
        std::vector<std::map<std::string, Node*> > m_uses;

        Symbol(Grammar* grammar,
               const std::string& key,
               const std::string& raw_rules);

        Symbol(Grammar* grammar,
               const std::string& key,
               const std::vector<std::string>& raw_rules);

        void clear_state();

        void push_rules(std::vector<std::string> raw_rules);

        void pop_rules();

        std::string select_rule(Node* node,
                                std::vector<std::string>& errors);

        std::string get_active_rules();
    };

    class Grammar
    {
       public:
        std::string m_raw;
        std::map<std::string, modifiers::modifier_func> m_modifiers;
        std::vector<std::string> m_errors;
        std::map<std::string, std::string> m_settings;
        std::map<std::string, Symbol*> m_symbols;

        Grammar(const std::string& raw,
                std::map<std::string, std::string> settings = std::map<std::string, std::string>());

        void clear_state();

        void add_modifiers(std::map<std::string,
                           modifiers::modifier_func> mods);

        void load_from_raw_obj(const std::string& raw);

        Node* create_root(const std::string& rule);

        Node* expand(const std::string& rule,
                     bool allow_escape_chars = false);

        std::string flatten(const std::string& rule,
                            bool allow_escape_chars = false);

        void push_rules(const std::string& key,
                        const std::vector<std::string>& raw_rules,
                        NodeAction* source_action = nullptr);

        void pop_rules(const std::string& key);

        std::string select_rule(const std::string& key,
                                Node* node,
                                std::vector<std::string>& errors);
    };

    class Parser
    {
       public:
        int m_last_escaped_char;
        std::string m_escaped_substring;

        void parse_tag(const std::string& tag_contents,
                       std::string* symbol,
                       std::vector<std::map<std::string, std::string> >* preactions,
                       std::vector<std::map<std::string, std::string> >* postactions,
                       std::vector<std::string>* modifiers);

        void create_section(const std::string& rule,
                            size_t start,
                            size_t end,
                            int type_,
                            std::vector<std::map<std::string, std::string> >* sections,
                            std::vector<std::string>* errors);

        void parse(const std::string& rule,
                   std::vector<std::map<std::string, std::string> >* sections,
                   std::vector<std::string>* errors);
    };
}  // namespace tracery
#endif  // TRACERY_TRACERY_H_
