#include <SFML/Graphics.hpp>
#include "entity_manager.h"

class Game {
    sf::Font m_font;
    unsigned long m_currentFrame = 0;
    unsigned long m_nextSpawnFrame = 100;
    sf::RenderWindow m_window;
    EntityManager m_entities;
    std::shared_ptr<Entity> player = nullptr;
    bool m_paused = false;

public:
    Game(const std::string &filename);
    void init();

    // Systems
    void sInput();
    void sLife();
    void sMovement();
    void sRender();
    void sCollision();
    void sEnemySpawner();
};