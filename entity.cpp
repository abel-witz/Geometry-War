#include "entity.h"

Entity::Entity(const std::string &tag) 
    : m_tag(tag)
{}

size_t Entity::id() {
    return m_id;
}

const std::string& Entity::tag() {
    return m_tag;
}

void Entity::destroy() {
    m_active = false;
}

bool Entity::active() {
    return m_active;
}