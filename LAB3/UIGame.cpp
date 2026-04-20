#include "UIGame.hpp"

UIGame::UIGame() 
    : window(sf::VideoMode(400, 400), "Wumpus World") {}

bool UIGame::isOpen() {
    return window.isOpen();
}

void UIGame::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void UIGame::render(int agentX, int agentY, const Perception& p) {
    window.clear();

    // Dibujar grid
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
            cell.setPosition(i * cellSize, (3 - j) * cellSize);
            cell.setFillColor(sf::Color::Black);
            cell.setOutlineThickness(2);
            cell.setOutlineColor(sf::Color::White);

            // Colorear celda actual según percepción
            if (i == agentX && j == agentY) {
                if (p.glitter)
                    cell.setFillColor(sf::Color::Yellow);
                else if (p.stench)
                    cell.setFillColor(sf::Color::Red);
                else if (p.breeze)
                    cell.setFillColor(sf::Color::Blue);
                else
                    cell.setFillColor(sf::Color(50, 50, 50));
            }

            window.draw(cell);
        }
    }

    sf::CircleShape agent(20);
    agent.setFillColor(sf::Color::Green);
    agent.setPosition(agentX * cellSize + 30, (3 - agentY) * cellSize + 30);

    window.draw(agent);

    window.display();
}