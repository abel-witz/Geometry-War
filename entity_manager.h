#pragma once
#include <vector>
#include <map>
#include "entity.h"


typedef std::vector<std::shared_ptr<Entity>> EntityVector;
typedef std::map<std::string, EntityVector> EntityMap;

class EntityManager {
    size_t m_total_entities = 0;
    EntityVector m_entities;
    EntityMap m_entityMap;
    EntityVector m_toAdd;

public:
    void update();
    std::shared_ptr<Entity> addEntity(const std::string& tag);
    EntityVector& getEntities();
    EntityVector& getEntities(const std::string& tag);
};