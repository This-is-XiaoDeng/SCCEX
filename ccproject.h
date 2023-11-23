#ifndef CCPROJECT_H
#define CCPROJECT_H

#include <iostream>
#include <vector>
#include "generator.h"
#include <json/json.h>

struct CcStoryNode {
    int type;
    std::string content;
    std::vector<CcStoryNode> children;
};

struct CcEvent {
    std::string name;
    std::string description;
    std::vector<End> ends;
    std::vector<CcStoryNode> tree;
};

struct OperationResult {
    bool is_success;
    std::string error;
};


class CcProject
{
public:
    CcProject();
    OperationResult load_project();
    OperationResult save_project();

    std::string file;
    std::vector<CcEvent> evnets;

private:
    OperationResult get_events(Json::Value root);
    Json::Value parse_events2json();
};

bool check_tree_node_keys(Json::Value node);
bool check_event_keys(Json::Value event);
std::vector<End> get_event_ends(Json::Value ends);
std::vector<CcStoryNode> get_evnet_tree(Json::Value json_tree);
Json::Value get_json_ends(std::vector<End> ends);
Json::Value get_json_tree(std::vector<CcStoryNode> nodes);
inline bool check_keys(Json::Value json_object, std::vector<std::string> needed_keys);

#endif // CCPROJECT_H
