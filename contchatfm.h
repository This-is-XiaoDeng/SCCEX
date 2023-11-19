#ifndef CONTCHATFM_H
#define CONTCHATFM_H

#include <iostream>
#include "generator.h"
#include <QTreeWidget>
#include <json/json.h>


struct ContChatProject {
    std::string name;
    std::string description;
    std::vector<EndWithStdString> ends;
    QTreeWidget *story_tree;
};

void save_project(ContChatProject project, std::string path);
std::string get_project_json(ContChatProject project);
Json::Value get_ends_json(std::vector<EndWithStdString> ends);
Json::Value gengerate_story_tree(std::vector<QTreeWidgetItem*> tree_items);
Json::Value get_story_tree(QTreeWidget *tree_widget);

#endif // CONTCHATFM_H
