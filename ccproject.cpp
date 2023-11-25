#include "ccproject.h"
#include <fstream>

CcProject::CcProject() {}

OperationResult CcProject::load_project()
{
    OperationResult result;
    std::ifstream file(this->file);
    if (!file.is_open()) {
        result.is_success = false;
        result.error = "打开文件失败：可能是因为文件不存在或权限不足";
        return result;
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(file, root, false)) {
        result.is_success = false;
        result.error = "解析项目时出现错误：JSON 结构不正确\n" + reader.getFormattedErrorMessages();
        return result;
    }

    return this->get_events(root);
}


std::vector<End> get_event_ends(Json::Value ends)
{
    std::vector<End> end_list;
    for (const std::string &end_id : ends.getMemberNames()) {
        End end;
        end.id = end_id;
        end.description = ends[end_id].asString();
        end_list.push_back(end);
    }
    return end_list;
}

inline bool check_keys(Json::Value json_object, std::vector<std::string> needed_keys)
{
    for (const std::string &key : needed_keys) {
        if (!json_object.isMember(key)) {
            return false;
        }
    }
    return true;
}

bool check_event_keys(Json::Value event)
{
    const std::vector<std::string> needed_keys = {"name", "description", "ends", "tree"};
    return check_keys(event, needed_keys);
}

bool check_tree_node_keys(Json::Value node)
{
    const std::vector<std::string> needed_keys = {"type", "content", "children"};
    return check_keys(node, needed_keys);
}

std::vector<CcStoryNode> get_evnet_tree(Json::Value json_tree)
{
    std::vector<CcStoryNode> nodes;
    if (!json_tree.isArray()) {
        return nodes;
    }
    for (const Json::Value &item : json_tree) {
        if (!check_tree_node_keys(item)) {
            continue;
        }
        CcStoryNode node;
        node.type = item["type"].asInt();
        node.content = item["type"].asString();
        node.children = get_evnet_tree(item["children"]);
        nodes.push_back(node);
    }
    return nodes;
}


OperationResult CcProject::get_events(Json::Value root)
{
    OperationResult result;
    if (!root.isArray()) {
        result.is_success = false;
        result.error = "类型不正确（应为 Array）";
        return result;
    }
    for (const Json::Value &event : root) {
        CcEvent ccevent;
        if (!check_event_keys(event)) {
            continue;
        }
        ccevent.name = event["name"].asString();
        ccevent.description = event["description"].asString();
        ccevent.ends = get_event_ends(event["ends"]);
        ccevent.tree = get_evnet_tree(event["tree"]);
        this->events.push_back(ccevent);
    }
    result.is_success = true;
    return result;
}


OperationResult CcProject::save_project()
{
    OperationResult result;
    std::ofstream file(this->file);
    if (!file.is_open()) {
        result.is_success = false;
        result.error = "打开文件失败：可能是因为文件不存在或权限不足";
        return result;
    }
    file << this->parse_events2json().toStyledString();
    file.close();
    result.is_success = true;
    return result;
}


Json::Value get_json_ends(std::vector<End> ends)
{
    Json::Value json_ends;
    for (const End &end : ends) {
        json_ends[end.id] = end.description;
    }
    return json_ends;
}

Json::Value get_json_tree(std::vector<CcStoryNode> nodes)
{
    Json::Value json_tree(Json::arrayValue);
    for (const CcStoryNode &node : nodes) {
        Json::Value json_node;
        json_node["type"] = node.type;
        json_node["content"] = node.content;
        json_node["children"] = get_json_tree(node.children);
        json_tree.append(json_node);
    }
    return json_tree;
}

Json::Value CcProject::parse_events2json()
{
    Json::Value events(Json::arrayValue);
    for (const CcEvent event : this->events) {
        Json::Value json_event;
        json_event["name"] = event.name;
        json_event["description"] = event.description;
        json_event["ends"] = get_json_ends(event.ends);
        json_event["tree"] = get_json_tree(event.tree);
        events.append(json_event);
    }
    return events;
}
