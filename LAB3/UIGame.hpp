#ifndef UI_GAME_HPP
#define UI_GAME_HPP

#include <SFML/Graphics.hpp>
#include "GameState.hpp"

class UIGame 
{
private:
    sf::RenderWindow window;
    int cellSize = 100;

public:
    UIGame();

    bool isOpen();
    void handleEvents();
    void render(int agentX, int agentY, const Perception& p);
};

#endif