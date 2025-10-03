#include "vec2.h"
#include <SFML/Graphics.hpp>

class CTransform {
public:
    Vec2 position;
    Vec2 speed;
    double angle;
};

class CShape {
public:
    float radius = 0;
    int vertices;
    sf::Color color;
};

class CLifespan {
public:
    int timeElapsed = 0;
    int lifeTime;
};

class CInput {
};

class CCollision {
public:
    float radius;
};

class CScore {
public:
    unsigned long score = 0;
};