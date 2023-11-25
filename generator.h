#ifndef GENERATOR_H
#define GENERATOR_H
#include <iostream>
#include "ccproject.h"

class Generator
{
public:
    Generator(CcProject project);

    std::string get_code();

private:
    unsigned int currect_indent_layer;
    std::string code;

    void new_line();
    void generate_event(CcEvent event);
    void generate_event_tree(std::vector<CcStoryNode> tree);
    void generate_tree_node(CcStoryNode node, int prev_node_type);

    void parse_message_node(CcStoryNode node);
    void parse_choice_node(CcStoryNode node, bool use_elif);
    void parse_if_node(CcStoryNode node, bool use_elif);
    void parse_code_node(CcStoryNode node);
    void parse_finish_node(CcStoryNode node);
};

std::string get_ends_string(CcEvent event);
std::string get_message_choices(std::vector<CcStoryNode> nodes);

#endif // GENERATOR_H
