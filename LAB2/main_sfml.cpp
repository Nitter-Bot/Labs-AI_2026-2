#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Algorithms.hpp"
#include "Heuristics.hpp"
#include <thread>
#include <chrono>

const int WINDOW_SIZE = 600;

void drawBoard(sf::RenderWindow& window, const State& state) {
    int tileSize = WINDOW_SIZE / state.size;

    for (int i = 0; i < state.n_tiles; ++i) {
        int val = state.board[i];

        sf::RectangleShape tile(sf::Vector2f(tileSize - 5, tileSize - 5));
        tile.setPosition((i % state.size) * tileSize, (i / state.size) * tileSize);

        if (val == 0) {
            tile.setFillColor(sf::Color::Black);
        } else {
            tile.setFillColor(sf::Color(100, 200, 250));
        }

        window.draw(tile);

        if (val != 0) {
            sf::Font font;
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(val));
            text.setCharacterSize(30);
            text.setFillColor(sf::Color::Black);

            text.setPosition(
                (i % state.size) * tileSize + tileSize / 3,
                (i / state.size) * tileSize + tileSize / 4
            );

            window.draw(text);
        }
    }
}

int main() {
    int size = 3;
    State start(size);

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Puzzle Solver");

    // Puedes cambiar heurística aquí
    auto heuristic = manhattanDistance;

    // Resolver en otro hilo
    std::vector<State> solution;
    bool solved = false;

    std::thread solver([&]() {
        AStarResult res = aStar(start, heuristic);
        if (res.success) {
            solution = res.path;
            solved = true;
        }
    });

    int step = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        if (solved && step < solution.size()) {
            if (clock.getElapsedTime().asMilliseconds() > 500) {
                step++;
                clock.restart();
            }
            drawBoard(window, solution[step]);
        } else {
            drawBoard(window, start);
        }

        window.display();
    }

    solver.join();
    return 0;
}