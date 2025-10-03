#pragma once
#include <string>
#include <memory>
#include "components.h"

class EntityManager;

class Entity {
    size_t m_id;
    std::string m_tag;
    bool m_active = true;
    Entity(const std::string &tag);

public:
    size_t id();
    const std::string& tag();
    void destroy();
    bool active();

    std::shared_ptr<CTransform> cTransform = nullptr;
    std::shared_ptr<CShape> cShape = nullptr;
    std::shared_ptr<CLifespan> cLifespan = nullptr;
    std::shared_ptr<CInput> cInput = nullptr;
    std::shared_ptr<CCollision> cCollision = nullptr;
    std::shared_ptr<CScore> cScore = nullptr;

    friend class EntityManager;
};