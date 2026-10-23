#include "BehaviorTree.hpp"
#include <utility>

SelectorNode::SelectorNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {
}

BTResult SelectorNode::tick(const BTContext& context) const {
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (result.success) {
            return result; 
        }
    }
    return { false, "" }; 
}

SequenceNode::SequenceNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {
}

BTResult SequenceNode::tick(const BTContext& context) const {
    std::string last_command;
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (!result.success) {
            return { false, "" }; 
        }
        if (!result.command.empty()) {
            last_command = result.command;
        }
    }
    return { true, last_command }; 
}

ConditionNode::ConditionNode(std::string name)
    : m_name(std::move(name)) {
}

BTResult ConditionNode::tick(const BTContext& context) const {
    for (const auto& fact : context) {
        if (fact.first == m_name && fact.second != 0) {
            return { true, "" }; 
        }
    }
    return { false, "" };
}

ActionNode::ActionNode(std::string command)
    : m_command(std::move(command)) {
}

BTResult ActionNode::tick(const BTContext& context) const {
    return { true, m_command }; 
}

BehaviorTree::BehaviorTree(BTNodePtr root)
    : m_root(std::move(root)) {
}

std::string BehaviorTree::tick(const BTContext& context) const {
    if (!m_root) {
        return "";
    }

    BTResult result = m_root->tick(context);
    return result.success ? result.command : ""; 
}

