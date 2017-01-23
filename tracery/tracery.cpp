#include <map>
#include <string>
#include <vector>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include "tracery.h"

using namespace modifiers;

namespace tracery
{
    Node::Node(Grammar* parent,
               int child_index,
               std::map<std::string, std::string> settings)
    : m_grammar(parent)
    , m_parent(nullptr)
    , m_depth(0)
    , m_child_index(0)
    , m_raw()
    , m_type(0)
    , m_is_expanded(false)
    , m_errors()
    , m_children()
    , m_finished_text()
    , m_child_rule()
    , m_symbol()
    , m_preactions()
    , m_postactions()
    , m_modifiers()
    , m_action(nullptr)
    {
        if (settings.count("raw") == 0)
        {
            m_errors.push_back("Empty input for node");
            settings["raw"].clear();
        }

        m_raw = settings["raw"];
        if (settings.count("type"))
        {
            m_type = atoi(settings["type"].c_str());
        }
    }

    Node::Node(Node* parent,
               int child_index,
               std::map<std::string, std::string> settings)
    : m_grammar(parent->m_grammar)
    , m_parent(parent)
    , m_depth(parent->m_depth + 1)
    , m_child_index(child_index)
    , m_raw()
    , m_type(0)
    , m_is_expanded(false)
    , m_errors()
    , m_children()
    , m_finished_text()
    , m_child_rule()
    , m_symbol()
    , m_preactions()
    , m_postactions()
    , m_modifiers()
    , m_action(nullptr)
    {
        if (settings.count("raw") == 0)
        {
            m_errors.push_back("Empty input for node");
            settings["raw"] = "";
        }

        m_raw = settings["raw"];
        if (settings.count("type"))
        {
            m_type = atoi(settings["type"].c_str());
        }
    }

    void Node::expand_children(const std::string& child_rule,
                               bool prevent_recursion)
    {
        m_children.clear();
        m_finished_text.clear();

        m_child_rule = child_rule;
        if (!m_child_rule.empty())
        {
            std::vector<std::map<std::string, std::string> > sections;
            std::vector<std::string> errors;
            Parser parser;
            parser.parse(child_rule, &sections, &errors);
            m_errors.insert(m_errors.end(), errors.begin(), errors.end());

            int i = 0;
            for (std::vector<std::map<std::string, std::string> >::iterator section = sections.begin();
                 section != sections.end();
                 ++section, ++i)
            {
                Node* node = new Node(this, i, *section);
                m_children.push_back(node);
                if (!prevent_recursion)
                {
                    node->expand(prevent_recursion);
                }
                m_finished_text += node->m_finished_text;
            }
        }
        else
        {
            m_errors.push_back("No child rule provided, can't expand children");
        }
    }

    void Node::expand(bool prevent_recursion)
    {
        if (!m_is_expanded)
        {
            m_is_expanded = true;
            // Types of nodes
            // -1: raw, needs parsing
            //  0: Plaintext
            //  1: Tag ("//symbol.mod.mod2.mod3//" or
            //     "//[pushTarget:pushRule]symbol.mod")
            //  2: Action ("[pushTarget:pushRule], [pushTarget:POP]",
            //     more in the future)
            if (m_type == -1)
            {
                expand_children(m_raw, prevent_recursion);
            }
            else if (m_type == 0)
            {
                m_finished_text = m_raw;
            }
            else if (m_type == 1)
            {
                m_preactions.clear();
                m_postactions.clear();

                Parser parser;
                std::vector<std::map<std::string, std::string> > preactions;
                std::vector<std::map<std::string, std::string> > postactions;
                parser.parse_tag(m_raw, &m_symbol, &preactions, &postactions, &m_modifiers);
                for (std::vector<std::map<std::string, std::string> >::iterator preaction = preactions.begin();
                     preaction != preactions.end();
                     ++preaction)
                {
                    m_preactions.push_back(new NodeAction(this, (*preaction)["raw"]));
                }
                for (std::vector<NodeAction*>::iterator preaction = m_preactions.begin();
                     preaction != m_preactions.end();
                     ++preaction)
                {
                    if ((*preaction)->m_type == 0)
                    {
                        m_postactions.push_back((*preaction)->create_undo());
                    }
                }
                for (std::vector<NodeAction*>::iterator preaction = m_preactions.begin();
                     preaction != m_preactions.end();
                     ++preaction)
                {
                    (*preaction)->activate();
                }
                m_finished_text = m_raw;
                std::string selected_rule = m_grammar->select_rule(m_symbol, this, m_errors);
                expand_children(selected_rule, prevent_recursion);

                // apply modifiers
                for (std::vector<std::string>::iterator mod_name = m_modifiers.begin();
                     mod_name != m_modifiers.end();
                     ++mod_name)
                {
                    std::vector<std::string> mod_params;
                    size_t openbracket = mod_name->find("(");
                    if (openbracket != std::string::npos)
                    {
                        std::vector<std::string> matches;
                        while (openbracket != std::string::npos)
                        {
                            ++openbracket;
                            size_t closebracket = mod_name->find(")", openbracket);
                            matches.push_back(mod_name->substr(openbracket, closebracket - openbracket));
                            openbracket = mod_name->find("(", closebracket + 1);
                        }
                        if (matches.size() > 0)
                        {
                            mod_params = split(matches[0], ",");
                            *mod_name = mod_name->substr(0, mod_name->find("("));
                        }
                    }
                    modifier_func mod = m_grammar->m_modifiers[*mod_name];
                    if (mod == nullptr)
                    {
                        m_errors.push_back("Missing modifier " + *mod_name);
                        m_finished_text += "((." + *mod_name + "))";
                    }
                    else
                    {
                        m_finished_text = (*mod)(m_finished_text, mod_params);
                    }
                }
            }
            else if (m_type == 2)
            {
                m_action = new NodeAction(this, m_raw);
                m_action->activate();
                m_finished_text.clear();
            }
        }
    }

    void Node::clear_escape_chars()
    {
        size_t ptr = m_finished_text.find("\\\\");
        while (ptr != std::string::npos)
        {
            m_finished_text.replace(ptr, 2, "DOUBLEBACKSLASH");
            ptr = m_finished_text.find("\\\\");
        }
        ptr = m_finished_text.find("\\");
        while (ptr != std::string::npos)
        {
            m_finished_text.replace(ptr, 1, "");
            ptr = m_finished_text.find("\\");
        }
        ptr = m_finished_text.find("DOUBLEBACKSLASH");
        while (ptr != std::string::npos)
        {
            m_finished_text.replace(ptr, 15, "\\");
            ptr = m_finished_text.find("DOUBLEBACKSLASH");
        }
    }

    NodeAction::NodeAction(Node* node,
                           const std::string& raw)
    : m_node(node)
    , m_target()
    , m_type(0)
    , m_rule()
    , m_rule_sections()
    , m_finished_rules()
    {
        std::vector<std::string> sections = split(raw, ":");
        if (sections.size() == 1)
        {
            m_target = sections[0];
            m_type = 2;
        }
        else if (sections.size() >= 1)
        {
            m_target = sections[0];
            m_rule = sections[1];
            if (m_rule == "POP")
            {
                m_type = 1;
            }
        }
    }

    NodeAction* NodeAction::create_undo()
    {
        if (m_type == 0)
        {
            return new NodeAction(m_node, m_target + ":POP");
        }
        return nullptr;
    }

    void NodeAction::activate()
    {
        Grammar* grammar = m_node->m_grammar;
        if (m_type == 0)
        {
            m_rule_sections = split(m_rule, ",");
            m_finished_rules.clear();
            for (std::vector<std::string>::iterator rule_section = m_rule_sections.begin();
                 rule_section != m_rule_sections.end(); ++rule_section)
            {
                std::map<std::string, std::string> settings;
                settings["type"] = "-1";
                settings["raw"] = *rule_section;
                Node* n = new Node(grammar, 0, settings);
                n->expand();
                m_finished_rules.push_back(n->m_finished_text);
            }
            grammar->push_rules(m_target, m_finished_rules, this);
        }
        else if (m_type == 1)
        {
            grammar->pop_rules(m_target);
        }
        else if (m_type == 2)
        {
            grammar->flatten(m_target, true);
        }
    }

    void NodeAction::to_text()
    {
    }

    RuleSet::RuleSet(Grammar* grammar,
                     const std::string& raw)
    : m_raw()
    , m_grammar(grammar)
    , m_default_uses()
    , m_default_rules()
    {
        m_raw.push_back(raw);
        m_default_rules.push_back(raw);
    }

    RuleSet::RuleSet(Grammar* grammar,
                     const std::vector<std::string>& raw)
    : m_raw(raw)
    , m_grammar(grammar)
    , m_default_uses()
    , m_default_rules(raw)
    {
    }

    std::string RuleSet::select_rule()
    {
        // in kate's code there's a bunch of stuff for different methods of
        // selecting a rule, none of which seem to be implemented yet! so for
        // now I'm just going to ...
        if (m_default_rules.empty())
        {
            return "";
        }
        return m_default_rules[rand() % m_default_rules.size()];
    }

    void RuleSet::clear_state()
    {
        m_default_uses.clear();
    }

    Symbol::Symbol(Grammar* grammar,
                   const std::string& key,
                   const std::string& raw_rules)
    : m_grammar(grammar)
    , m_key(key)
    , m_raw_rules()
    , m_base_rules(nullptr)
    , m_stack()
    , m_uses()
    {
        m_raw_rules.push_back(raw_rules);
        m_base_rules = new RuleSet(grammar, raw_rules);
        clear_state();
    }

    Symbol::Symbol(Grammar* grammar,
                   const std::string& key,
                   const std::vector<std::string>& raw_rules)
    : m_grammar(grammar)
    , m_key(key)
    , m_raw_rules(raw_rules)
    , m_base_rules(nullptr)
    , m_stack()
    , m_uses()
    {
        m_base_rules = new RuleSet(grammar, raw_rules);
        clear_state();
    }

    void Symbol::clear_state()
    {
        m_stack.clear();
        m_stack.push_back(m_base_rules);
        m_uses.clear();
        m_base_rules->clear_state();
    }

    void Symbol::push_rules(std::vector<std::string> raw_rules)
    {
        RuleSet* rules = new RuleSet(m_grammar, raw_rules);
        m_stack.push_back(rules);
    }

    void Symbol::pop_rules()
    {
        m_stack.erase(m_stack.end() - 1);
    }

    std::string Symbol::select_rule(Node* node,
                                    std::vector<std::string>& errors)
    {
        std::map<std::string, Node*> use;
        use["node"] = node;
        m_uses.push_back(use);
        if (m_stack.empty())
        {
            errors.push_back("The rule stack for '" + m_key + "' is empty, too many pops?");
            return "";
        }
        return m_stack[m_stack.size() - 1]->select_rule();
    }

    std::string Symbol::get_active_rules()
    {
        if (m_stack.size() == 0)
        {
            return "";
        }
        return m_stack[m_stack.size() - 1]->select_rule();
    }

    Grammar::Grammar(const std::string& raw,
                     std::map<std::string, std::string> settings)
    : m_raw()
    , m_modifiers()
    , m_errors()
    , m_settings(settings)
    , m_symbols()
    {
        load_from_raw_obj(raw);
    }

    void Grammar::clear_state()
    {
        for (std::map<std::string, Symbol*>::iterator val = m_symbols.begin(); val != m_symbols.end(); ++val)
        {
            val->second->clear_state();
        }
    }

    void Grammar::add_modifiers(std::map<std::string, modifier_func> mods)
    {
        // not sure what this is for yet
        for (std::map<std::string, modifier_func>::const_iterator key = mods.begin(); key != mods.end(); ++key)
        {
            m_modifiers[key->first] = mods[key->first];
        }
    }

    void Grammar::load_from_raw_obj(const std::string& raw)
    {
        m_raw = raw;
        m_symbols.clear();
        if (!raw.empty())
        {
            const char* return_parse_end;
            cJSON* json = cJSON_ParseWithOpts(raw.c_str(), &return_parse_end, true);
            if (json)
            {
                cJSON* ptr = json->child;
                while (ptr)
                {
                    if (ptr->child)
                    {
                        std::vector<std::string> values;
                        cJSON* v = ptr->child;
                        while (v)
                        {
                            values.push_back(v->valuestring);
                            v = v->next;
                        }
                        m_symbols[ptr->string] = new Symbol(this, ptr->string, values);
                    }
                    else
                    {
                        m_symbols[ptr->string] = new Symbol(this, ptr->string, ptr->valuestring);
                    }
                    ptr = ptr->next;
                }
                cJSON_Delete(json);
            }
        }
    }

    Node* Grammar::create_root(const std::string& rule)
    {
        std::map<std::string, std::string> settings;
        settings["type"] = "-1";
        settings["raw"] = rule;
        return new Node(this, 0, settings);
    }

    Node* Grammar::expand(const std::string& rule,
                          bool allow_escape_chars)
    {
        Node* root = create_root(rule);
        root->expand();
        if (!allow_escape_chars)
        {
            root->clear_escape_chars();
        }
        m_errors.insert(m_errors.end(), root->m_errors.begin(), root->m_errors.end());
        return root;
    }

    std::string Grammar::flatten(const std::string& rule,
                                 bool allow_escape_chars)
    {
        Node* root = expand(rule, allow_escape_chars);
        return root->m_finished_text;
    }

    void Grammar::push_rules(const std::string& key,
                             const std::vector<std::string>& raw_rules,
                             NodeAction* source_action)
    {
        if (!m_symbols.count(key))
        {
            m_symbols[key] = new Symbol(this, key, raw_rules);
        }
        else
        {
            m_symbols[key]->push_rules(raw_rules);
        }
    }

    void Grammar::pop_rules(const std::string& key)
    {
        if (!m_symbols.count(key))
        {
            m_errors.push_back("Can't pop: no symbol for key " + key);
        }
        else
        {
            m_symbols[key]->pop_rules();
        }
    }

    std::string Grammar::select_rule(const std::string& key, Node* node,
                                     std::vector<std::string>& errors)
    {
        if (m_symbols.count(key))
        {
            return m_symbols[key]->select_rule(node, errors);
        }
        else
        {
            errors.push_back("No symbol for " + key);
            return "((" + key + "))";
        }
    }

    void Parser::parse_tag(const std::string& tag_contents,
                           std::string* symbol,
                           std::vector<std::map<std::string, std::string> >* preactions,
                           std::vector<std::map<std::string, std::string> >* postactions,
                           std::vector<std::string>* modifiers)
    {
        //
        // returns a dictionary with 'symbol', 'modifiers', 'preactions', 'postactions'
        //
        *symbol = "None";
        std::vector<std::map<std::string, std::string> > sections;
        std::vector<std::string> errors;
        parse(tag_contents, &sections, &errors);

        std::string symbol_section;
        for (std::vector<std::map<std::string, std::string> >::iterator section = sections.begin();
             section != sections.end(); ++section)
        {
            if ((*section)["type"] == "0")
            {
                if (symbol_section.empty())
                {
                    symbol_section = (*section)["raw"];
                }
                else
                {
                    assert(!"multiple main sections");  // tag_contents
                }
            }
            else
            {
                preactions->push_back(*section);
            }
        }
        if (!symbol_section.empty())
        {
            std::vector<std::string> components = split(symbol_section, ".");
            *symbol = components[0];
            components.erase(components.begin());
            *modifiers = components;
        }
    }

    void Parser::create_section(const std::string& rule,
                                size_t start,
                                size_t end,
                                int type_,
                                std::vector<std::map<std::string, std::string> >* sections,
                                std::vector<std::string>* errors)
    {
        if (end - start < 1)
        {
            char start_str[8];
            snprintf(start_str, sizeof(start_str), "%zu", start);

            if (type_ == 1)
            {
                errors->push_back(std::string(start_str) + ": empty tag");
            }
            else if (type_ == 2)
            {
                errors->push_back(std::string(start_str) + ": empty action");
            }
        }
        std::string raw_substring;
        if (m_last_escaped_char != -1)
        {
            raw_substring =
                m_escaped_substring + "\\" + rule.substr(m_last_escaped_char + 1, end - (m_last_escaped_char + 1));
        }
        else
        {
            raw_substring = rule.substr(start, end - start);
        }
        std::map<std::string, std::string> section;
        char type_str[5];
        snprintf(type_str, sizeof(type_str), "%d", type_);
        section["type"] = type_str;
        section["raw"] = raw_substring;
        sections->push_back(section);
    }

    void Parser::parse(const std::string& rule,
                       std::vector<std::map<std::string, std::string> >* sections,
                       std::vector<std::string>* errors)
    {
        int depth = 0;
        bool in_tag = false;
        sections->clear();
        bool escaped = false;
        errors->clear();
        size_t start = 0;
        m_escaped_substring.clear();
        m_last_escaped_char = -1;

        if (rule.empty())
        {
            return;
        }

        for (size_t i = 0; i < rule.length(); ++i)
        {
            char c = rule.at(i);
            if (!escaped)
            {
                if (c == '[')
                {
                    if (depth == 0 && !in_tag)
                    {
                        if (start < i)
                        {
                            create_section(rule, start, i, 0, sections, errors);
                            m_last_escaped_char = -1;
                            m_escaped_substring.clear();
                        }
                        start = i + 1;
                    }
                    depth += 1;
                }
                else if (c == ']')
                {
                    depth -= 1;
                    if (depth == 0 && !in_tag)
                    {
                        create_section(rule, start, i, 2, sections, errors);
                        m_last_escaped_char = -1;
                        m_escaped_substring.clear();
                        start = i + 1;
                    }
                }
                else if (c == '#')
                {
                    if (depth == 0)
                    {
                        if (in_tag)
                        {
                            create_section(rule, start, i, 1, sections, errors);
                            m_last_escaped_char = -1;
                            m_escaped_substring.clear();
                            start = i + 1;
                        }
                        else
                        {
                            if (start < i)
                            {
                                create_section(rule, start, i, 0, sections, errors);
                                m_last_escaped_char = -1;
                                m_escaped_substring.clear();
                            }
                            start = i + 1;
                        }
                        in_tag = !in_tag;
                    }
                }
                else if (c == '\\')
                {
                    escaped = true;
                    m_escaped_substring += rule.substr(start, i - start);
                    start = i + 1;
                    m_last_escaped_char = i;
                }
            }
            else
            {
                escaped = false;
            }
        }
        if (start < rule.size())
        {
            create_section(rule, start, rule.length(), 0, sections, errors);
            m_last_escaped_char = -1;
            m_escaped_substring.clear();
        }

        if (in_tag)
        {
            errors->push_back("unclosed tag");
        }
        if (depth > 0)
        {
            errors->push_back("too many [");
        }
        if (depth < 0)
        {
            errors->push_back("too many ]");
        }

        std::vector<std::map<std::string, std::string> > stripped_sections;
        for (std::vector<std::map<std::string, std::string> >::iterator s = sections->begin();
             s != sections->end();
             ++s)
        {
            if (!((*s)["type"] == "0" && (*s)["raw"].length() == 0))
            {
                stripped_sections.push_back(*s);
            }
        }
        *sections = stripped_sections;
    }
}
