#include "contchatfm.h"
#include <fstream>



void save_project(ContChatProject project, std::string path) {
    // std::cout << get_project_json(project) << std::endl;
    std::ofstream file(path);
    if (file.is_open()) {
        file << get_project_json(project);
        file.close();
    } else {
        std::cerr << "Error opening the file." << std::endl;
    }

}

std::string get_project_json(ContChatProject project)
{
    Json::Value root;
    root["name"] = project.name;
    root["description"] = project.description;
    root["ends"] = get_ends_json(project.ends);
    root["tree"] = get_story_tree(project.story_tree);
    return root.toStyledString();
}

Json::Value get_ends_json(std::vector<EndWithStdString> ends)
{
    Json::Value cc_ends;
    for (const auto &end : ends) {
        cc_ends[end.id] = end.description;
    }
    return cc_ends;
}

Json::Value get_story_tree(QTreeWidget *tree_widget)
{
    std::vector<QTreeWidgetItem*> top_level_items;
    for (int i=0; i<tree_widget->topLevelItemCount(); i++) {
        top_level_items.push_back(tree_widget->topLevelItem(i));
    }
    return gengerate_story_tree(top_level_items);
}

Json::Value gengerate_story_tree(std::vector<QTreeWidgetItem*> tree_items)
{
    Json::Value tree(Json::arrayValue);
    for (const auto &item : tree_items)
    {
        Json::Value tree_item;
        tree_item["type"] = item->text(0).toStdString();
        tree_item["content"] = item->text(1).toStdString();
        std::vector<QTreeWidgetItem*> children;
        for (int i=0; i < item->childCount(); i++)
        {
            children.push_back(item->child(i));
        }
        tree_item["children"] = gengerate_story_tree(children);
        tree.append(tree_item);
    }
    return tree;
}


