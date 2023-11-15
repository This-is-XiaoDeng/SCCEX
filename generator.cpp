#include "generator.h"
#include <QTreeWidget>

std::string generate_choice_list(std::vector<std::string> choices)
{
    std::string choice_list = "[";
    for (const auto &choice : choices)
    {
        choice_list += "r'''" + choice + "''', ";
    }
    if (choice_list.back() == '[') {
        return "";
    }
    return choice_list + "]";
}

Generator::Generator(std::string name, std::string description)
{
    this->code = "from .ContChat import *\n\n";
    this->code += "@NewTestContChat(r'''" + name + "'''," + "r'''" + description  + "''')\n";
    this->code += "async def _(event: ContChatEvent):\n";
    this->currect_indent_layer = 1;
}

void Generator::generate(QTreeWidget* tree_widget)
{
    std::vector<QTreeWidgetItem*> top_level_items = {};
    for (int i = 0; i < tree_widget->topLevelItemCount(); i++)
    {
       top_level_items.push_back(tree_widget->topLevelItem(i));
    }
    this->generate_by_tree_items(top_level_items);
}

std::vector<QTreeWidgetItem*> get_child_items(QTreeWidgetItem *item)
{
    std::vector<QTreeWidgetItem*> child_items = {};
    for (int i=0; i<item->childCount(); i++)
    {
        child_items.push_back(item->child(i));
    }
    return child_items;
}


std::vector<std::string> get_message_choices(QTreeWidgetItem *item)
{
    std::vector<std::string> choices = {};
    for (const auto &chlid_item : get_child_items(item))
    {
        if (chlid_item->text(0).toStdString() == "选项")
        {
            choices.push_back(chlid_item->text(1).toStdString());
        }
    }
    return choices;
}

std::string Generator::get_code()
{
    return this->code;
}

void Generator::generate_by_tree_items(std::vector<QTreeWidgetItem*> items)
{
    bool use_elif = false;
    for (const auto &item : items)
    {
       this->init_line();
       const std::string item_type = item->text(0).toStdString();
       if (item_type != "选项") {
            use_elif = false;
       }
       if (item_type == "选项") {
            this->code += std::string(use_elif? "elif":"if") + " event.choice == r'''";
            this->code += item->text(1).toStdString() + "''':";
            use_elif = true;
            this->currect_indent_layer++;
            this->generate_by_tree_items(get_child_items(item));
            this->currect_indent_layer--;
       } else if (item_type == "代码") {
            this->code += item->text(1).toStdString();
       } else if (item_type == "判定") {
            this->code += "if " + item->text(1).toStdString() + ":";
            this->currect_indent_layer++;
            this->generate_by_tree_items(get_child_items(item));
            this->currect_indent_layer--;
       } else if (item_type == "结束") {
            this->code += "yield event.Finish()";     // TODO 结局 ID
            return;
       } else if (item_type == "消息") {
            this->code += "yield event.PushMsg(r'''" + item->text(1).toStdString() + "''',";
            this->code += generate_choice_list(get_message_choices(item)) + ")";
            this->generate_by_tree_items(get_child_items(item));
       }
    }
}

void Generator::init_line()
{
    if (this->code.back() != '\n')
    {
        this->code += "\n";
    }
    for (int i=0; i<this->currect_indent_layer; i++)
    {
        this->code += "    ";
    }
}
