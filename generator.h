#ifndef GENERATOR_H
#define GENERATOR_H
#include <QTreeWidget>
#include <iostream>
#include <QTableWidget>
#include <vector>

struct EndWithStdString {
    std::string id;
    std::string description;
};

class Generator
{
public:
    Generator(std::string name, std::string description, QTableWidget *ends);
    void generate(QTreeWidget* tree_widget);
    std::string get_code();
private:
    int currect_indent_layer;
    std::string code;

    void init_line();
    void generate_by_tree_items(std::vector<QTreeWidgetItem*> items);
};

std::string generate_choice_list(std::vector<std::string> choices);
std::vector<QTreeWidgetItem*> get_child_items(QTreeWidgetItem *item);
std::vector<std::string> get_message_choices(QTreeWidgetItem *item);
std::vector<EndWithStdString> get_ends(QTableWidget *ends);
std::string get_ends_list(std::vector<EndWithStdString> ends);

#endif // GENERATOR_H
