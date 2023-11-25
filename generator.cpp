#include "generator.h"
#include <fmt/format.h>

Generator::Generator(CcProject project)
{
    this->code = "from .ContChat import *\n";
    for (const CcEvent &event : project.events) {
        this->generate_event(event);
    }
}

std::string Generator::get_code()
{
    return this->code;
}

void Generator::new_line()
{
    this->code.append("\n");
    for (int i=0;i<this->currect_indent_layer;i++) {
        this->code.append("    ");
    }

}

void Generator::generate_event(CcEvent event)
{
    this->currect_indent_layer = 0;
    this->new_line();
    this->code.append(fmt::format(
        "@NewTestContChat(r'''{}''', r'''{}''', ends=[{}])",
        event.name,
        event.description,
        get_ends_string(event)
    ));
    this->new_line();
    this->code.append("def _(event: ContChatEvent):");
    this->currect_indent_layer++;
    this->generate_event_tree(event.tree);
}

void Generator::generate_event_tree(std::vector<CcStoryNode> tree)
{
    int prev_node_type = -1;
    for (const CcStoryNode &node : tree) {
        this->generate_tree_node(node, prev_node_type);
        prev_node_type = node.type;
    }
}

#define MESSAGE  0
#define CHOICE   1
#define CODE     2
#define IF       3
#define FINISH   4

void Generator::generate_tree_node(CcStoryNode node, int prev_node_type)
{
    this->new_line();
    switch (node.type) {
    case MESSAGE:
        this->parse_message_node(node);
        break;
    case CHOICE:
        this->parse_choice_node(node, prev_node_type == CHOICE);
        break;
    case CODE:
        this->parse_code_node(node);
        break;
    case IF:
        this->parse_if_node(node, prev_node_type == IF);
        break;
    case FINISH:
        this->parse_finish_node(node);
        break;
    }
}


std::string get_message_choices(std::vector<CcStoryNode> nodes)
{
    if (nodes.empty()) {
        return "";
    }
    std::string choices = "[";
    for (const CcStoryNode &node : nodes) {
        if (node.type != CHOICE) {
            continue;
        }
        choices.append(fmt::format(
            "r'''{}''',",
            node.content
        ));
    }
    choices.append("]");
    return choices;
}

#undef MESSAGE
#undef CHOICE
#undef CODE
#undef IF
#undef FINISH

void Generator::parse_message_node(CcStoryNode node)
{
    this->code.append(fmt::format(
        "yield event.PushMsg(r'''{}'''{})",
        node.content,
        get_message_choices(node.children)
    ));
    this->generate_event_tree(node.children);
}

void Generator::parse_choice_node(CcStoryNode node, bool use_elif)
{
    this->code.append(fmt::format(
        "{} event.choice == r'''{}''':",
        use_elif ? "elif" : "if",
        node.content
    ));
    this->currect_indent_layer++;
    if (node.children.empty()) {
        this->code.append(" pass");
    } else {
        this->generate_event_tree(node.children);
    }
    this->currect_indent_layer--;
}

void Generator::parse_code_node(CcStoryNode node)
{
    this->code.append(node.content);
}

void Generator::parse_if_node(CcStoryNode node, bool use_elif)
{
    this->code.append(fmt::format(
        "{} {}:",
        use_elif ? "elif" : "if",
        node.content
        ));
    this->currect_indent_layer++;
    if (node.children.empty()) {
        this->code.append(" pass");
    } else {
        this->generate_event_tree(node.children);
    }
    this->currect_indent_layer--;
}

void Generator::parse_finish_node(CcStoryNode node)
{
    this->code.append(fmt::format(
        "yield event.Finish({})",
        node.content.empty() ? "" : fmt::format("r'''{}'''", node.content)
    ));
}

std::string get_ends_string(CcEvent event)
{
    std::string ends_string;
    for (const End &end : event.ends) {
        ends_string.append(fmt::format(
            "CcEnd(r'''{}''', r'''{}''')",
            end.id,
            end.description
        ));
    }
    return ends_string;
}
