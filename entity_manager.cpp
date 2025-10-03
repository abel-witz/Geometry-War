#include "entity_manager.h"
#include <algorithm>
#include <iostream>

bool isMarkedToDelete(const std::shared_ptr<Entity>& entity) {
    return !entity->active();
}

void EntityManager::update() {
    for (auto e : m_toAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }

    m_toAdd = EntityVector();
    
    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), isMarkedToDelete), m_entities.end());
    
    for (auto& x : m_entityMap)
    {
        x.second.erase(std::remove_if(x.second.begin(), x.second.end(), isMarkedToDelete), x.second.end());
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    std::shared_ptr<Entity> newEntity = std::shared_ptr<Entity>(new Entity(tag));
    newEntity->m_id = m_total_entities++;
    m_toAdd.push_back(newEntity);
    return newEntity;
}

EntityVector& EntityManager::getEntities() {
    return m_entities;
};

EntityVector& EntityManager::getEntities(const std::string& tag) {
    return m_entityMap[tag];
}