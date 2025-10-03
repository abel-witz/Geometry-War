#include "game.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string& filename) {
    m_font = sf::Font("arial.ttf");

    std::ifstream fin(filename);
    
    while(fin.good()) {
        std::string option;
        fin >> option;

        if (option == "Window") {
            unsigned int width, height;
            fin >> width >> height;
            m_window.create(sf::VideoMode({width, height}), "Geometry War");
            m_window.setVerticalSyncEnabled(true);
            m_window.setFramerateLimit(60);
        }
    }
}

void Game::init() {
    srand(time(NULL));

    auto newEntity = m_entities.addEntity("Player");
    player = newEntity;
    newEntity->cTransform = std::make_shared<CTransform>();
    newEntity->cTransform->position = Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2);
    newEntity->cShape = std::make_shared<CShape>();
    newEntity->cShape->radius = 32;
    newEntity->cShape->vertices = 8;
    newEntity->cShape->color = {0,0,0};
    newEntity->cInput = std::make_shared<CInput>();
    newEntity->cCollision = std::make_shared<CCollision>();
    newEntity->cCollision->radius = 32;
    newEntity->cScore = std::make_shared<CScore>();

    while (m_window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = m_window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                m_window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    m_window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::P)
                    m_paused = !m_paused;
            } else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (!m_paused && mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    Vec2 fireVector = Vec2(mouseButtonPressed->position.x, mouseButtonPressed->position.y) - player->cTransform->position;
                    fireVector = fireVector.normalize();
                    fireVector *= 10;

                    auto newBullet = m_entities.addEntity("Bullet");
                    newBullet->cTransform = std::make_shared<CTransform>();
                    newBullet->cTransform->position = player->cTransform->position;
                    newBullet->cTransform->speed = fireVector;
                    newBullet->cShape = std::make_shared<CShape>();
                    newBullet->cShape->radius = 10;
                    newBullet->cShape->vertices = 1;
                    newBullet->cShape->color = {255,255,255};
                    newBullet->cLifespan = std::make_shared<CLifespan>();
                    newBullet->cLifespan->lifeTime = 60;
                    newBullet->cCollision = std::make_shared<CCollision>();
                    newBullet->cCollision->radius = 10;
                }
            }
        }

        // Systems
        if (!m_paused) {
            m_entities.update();
            sEnemySpawner();
            sInput();
            sLife();
            sMovement();
            sCollision();
            sRender();
            m_currentFrame++;
        }
    }
}

void Game::sInput() {
    // Player movement
    for (auto e : m_entities.getEntities()) {
        if (e->cInput) {
            e->cTransform->speed.x = 0;
            e->cTransform->speed.y = 0;

            if (e->cTransform) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
                {
                    e->cTransform->speed.y = -5;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
                {
                    e->cTransform->speed.x = -5;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                {
                    e->cTransform->speed.y = 5;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                {
                    e->cTransform->speed.x = 5;
                }
            }
        }
    }
}

void Game::sLife() {
    for (auto e : m_entities.getEntities()) {
        if (e->cLifespan) {
            e->cLifespan->timeElapsed += 1;

            if (e->cLifespan->timeElapsed >= e->cLifespan->lifeTime) {
                e->destroy();
            }
        }
    }
}

void Game::sMovement() {
    for (auto e : m_entities.getEntities()) {
        if (e->cTransform) {
            e->cTransform->position += e->cTransform->speed;
            e->cTransform->angle += 1.0;

            if (e->tag() != "Bullet") {
                float radius = 0;
                if (e->cShape) {
                    radius = e->cShape->radius;
                }

                if (e->cTransform->position.x - radius < 0) {
                    e->cTransform->position.x = radius;
                    e->cTransform->speed.x = -e->cTransform->speed.x;
                } else if (e->cTransform->position.x + radius > m_window.getSize().x) {
                    e->cTransform->position.x = m_window.getSize().x - radius;
                    e->cTransform->speed.x = -e->cTransform->speed.x;
                }

                if (e->cTransform->position.y - radius < 0) {
                    e->cTransform->position.y = radius;
                    e->cTransform->speed.y = -e->cTransform->speed.y;
                } else if (e->cTransform->position.y + radius > m_window.getSize().y) {
                    e->cTransform->position.y = m_window.getSize().y - radius;
                    e->cTransform->speed.y = -e->cTransform->speed.y;
                }
            }
        }
    }
}

void Game::sRender() {
    m_window.clear();

    for (auto e : m_entities.getEntities()) {
        if (e->cTransform && e->cShape) {
            sf::CircleShape shape;

            if (e->cShape->vertices == 1) {
                shape = sf::CircleShape(e->cShape->radius);
            } else {
                shape = sf::CircleShape(e->cShape->radius, e->cShape->vertices);
            }

            shape.setPosition({e->cTransform->position.x, e->cTransform->position.y});
            shape.setOrigin({e->cShape->radius, e->cShape->radius});
            shape.setRotation(sf::degrees(e->cTransform->angle));

            int opacity = 255;
            if (e->cLifespan) {
                opacity = 255-(255*e->cLifespan->timeElapsed / e->cLifespan->lifeTime);
            }

            shape.setFillColor({e->cShape->color.r, e->cShape->color.g, e->cShape->color.b, (std::uint8_t)opacity});
            if (e->tag() != "Bullet")
                shape.setOutlineThickness(2.f);
                
            if (e->tag() == "Player") {
                shape.setOutlineColor(sf::Color(255, 0, 0, opacity));
            } else {
                shape.setOutlineColor(sf::Color(255, 255, 255, opacity));
            }
            m_window.draw(shape);
        }
    }

    if (player->cScore) {
        sf::Text text(m_font);
        text.setString("Score: " + std::to_string(player->cScore->score));
        text.setCharacterSize(24);
        text.setFillColor({255,255,255});
        text.setPosition({10, 10});
        m_window.draw(text);
    }


    m_window.display();
}

void Game::sCollision() {
    for (auto e : m_entities.getEntities("Bullet")) {
        if (e->cTransform && e->cCollision) {
            for (auto e2 : m_entities.getEntities("Enemy")) {
                if (e2->cTransform && e2->cCollision) {
                    float distance = (e2->cTransform->position - e->cTransform->position).length();
                    float radiusSum = e->cCollision->radius + e2->cCollision->radius;
                    if (distance < radiusSum) {
                        if (e2->cShape) {
                            for (int i = 0; i < e2->cShape->vertices; i++) {
                                float angle = (360.0 / e2->cShape->vertices) * i;
                                float angleRad = angle * (3.14159265f / 180.0f);
                                float length = 10;
                                Vec2 direction = Vec2(cosf(angleRad), sinf(angleRad));
                                Vec2 position = e2->cTransform->position + direction;

                                auto miniEnemy = m_entities.addEntity("MiniEnemy");
                                miniEnemy->cTransform = std::make_shared<CTransform>();
                                miniEnemy->cTransform->position = position;
                                miniEnemy->cTransform->angle = e2->cTransform->angle;
                                miniEnemy->cTransform->speed = direction.normalize() * 2;
                                miniEnemy->cShape = std::make_shared<CShape>();
                                miniEnemy->cShape->radius = e2->cShape->radius * 0.5;
                                miniEnemy->cShape->vertices = e2->cShape->vertices;
                                miniEnemy->cShape->color = e2->cShape->color;
                                miniEnemy->cCollision = std::make_shared<CCollision>();
                                miniEnemy->cCollision->radius = e2->cShape->radius * 0.5;
                                miniEnemy->cLifespan = std::make_shared<CLifespan>();
                                miniEnemy->cLifespan->lifeTime = 60;

                            }

                            for (auto e3 : m_entities.getEntities()) {
                                if (e3->cScore) {
                                    e3->cScore->score += (int)((e2->cShape->radius / 50) * 700);
                                }
                            }
                        }
                        e->destroy();
                        e2->destroy();
                    }
                }
            }
        }
    }

    for (auto e : m_entities.getEntities("Bullet")) {
        if (e->cTransform && e->cCollision) {
            for (auto e2 : m_entities.getEntities("MiniEnemy")) {
                if (e2->cTransform && e2->cCollision) {
                    float distance = (e2->cTransform->position - e->cTransform->position).length();
                    float radiusSum = e->cCollision->radius + e2->cCollision->radius;
                    if (distance < radiusSum) {
                        if (e2->cShape) {
                            for (auto e3 : m_entities.getEntities()) {
                                if (e3->cScore) {
                                    e3->cScore->score += (int)((e2->cShape->radius / 50) * 700);
                                }
                            }
                        }
                        e->destroy();
                        e2->destroy();
                    }
                }
            }
        }
    }

    for (auto e : m_entities.getEntities("Enemy")) {
        if (e->cTransform && e->cCollision) {
            for (auto e2 : m_entities.getEntities("Player")) {
                if (e2->cTransform && e2->cCollision) {
                    float distance = (e2->cTransform->position - e->cTransform->position).length();
                    float radiusSum = e->cCollision->radius + e2->cCollision->radius;

                    if (distance < radiusSum) {
                        if (e2->cTransform) {
                            e2->cTransform->position = Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2);
                            e2->cTransform->angle = 0;
                        }

                        for (auto e3 : m_entities.getEntities()) {
                            if (e3->tag() != "Player") {
                                e3->destroy();
                            } else {
                                if (e3->cScore) {
                                    e3->cScore->score = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (auto e : m_entities.getEntities("MiniEnemy")) {
        if (e->cTransform && e->cCollision) {
            for (auto e2 : m_entities.getEntities("Player")) {
                if (e2->cTransform && e2->cCollision) {
                    float distance = (e2->cTransform->position - e->cTransform->position).length();
                    float radiusSum = e->cCollision->radius + e2->cCollision->radius;

                    if (distance < radiusSum) {
                        if (e2->cTransform) {
                            e2->cTransform->position = Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2);
                            e2->cTransform->angle = 0;
                        }

                        for (auto e3 : m_entities.getEntities()) {
                            if (e3->tag() != "Player") {
                                e3->destroy();
                            } else {
                                if (e3->cScore) {
                                    e3->cScore->score = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool near_other_entities(const EntityVector& entities, float x_position, float y_position, float radius) {
    for (auto e : entities) {
        if (e->cCollision && e->cTransform) {
            float dist = (Vec2(x_position, y_position) - e->cTransform->position).length();
            float radiusSum = e->cCollision->radius + radius;

            if (dist - 100 < radiusSum) {
                return true;
            }
        }
    }

    return false;
}

void Game::sEnemySpawner() {
    if (m_currentFrame >= m_nextSpawnFrame) {
        float radius = rand() % (1 + 50 - 20) + 20;
        float x_position = rand() % (1 + (m_window.getSize().x - 40) - 40) + 40;
        float y_position = rand() % (1 + (m_window.getSize().y - 40) - 40) + 40;

        while(near_other_entities(m_entities.getEntities(), x_position, y_position, radius)) {
            x_position = rand() % (1 + (m_window.getSize().x - 40) - 40) + 40;
            y_position = rand() % (1 + (m_window.getSize().y - 40) - 40) + 40;
        }

        float x_speed = rand() % (1 + 5 - (-5)) + -5;
        float y_speed = rand() % (1 + 5 - (-5)) + -5;
        u_int8_t r = rand() % (1 + 200 - 30) + 30;
        u_int8_t g = rand() % (1 + 200 - 30) + 30;
        u_int8_t b = rand() % (1 + 200 - 30) + 30;

        int vertices;
        int vertexChoice = rand() % (1 + 3 - 1) + 1;
        if (vertexChoice == 1) {
            vertices = 3;
        } else if (vertexChoice == 2) {
            vertices = 4;
        } else if (vertexChoice == 3) {
            vertices = 5;
        }

        std::shared_ptr<Entity> newEnemy = m_entities.addEntity("Enemy");
        newEnemy->cTransform = std::make_shared<CTransform>();
        newEnemy->cTransform->position = Vec2(x_position, y_position);
        newEnemy->cTransform->speed = Vec2(x_speed, y_speed);
        newEnemy->cShape = std::make_shared<CShape>();
        newEnemy->cShape->radius = radius;
        newEnemy->cShape->vertices = vertices;
        newEnemy->cShape->color = {r,g,b};
        newEnemy->cCollision = std::make_shared<CCollision>();
        newEnemy->cCollision->radius = 40;

        m_nextSpawnFrame = m_currentFrame + 100;
    }
}