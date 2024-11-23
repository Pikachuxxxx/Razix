#include "fg/GraphNode.hpp"

GraphNode::GraphNode(const std::string_view name, uint32_t id)
    : m_name{name}, m_id{id} {}
